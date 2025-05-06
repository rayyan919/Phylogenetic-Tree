import sys
from PyQt6 import QtWidgets, QtGui, QtCore
import pyqtgraph as pg

# Import the C++ backend bridge
import backend_bridge


class Node:
    def __init__(self, NodeType, Name, Parent=None, GeneticSequence=None, Weight=None):
        self.type = NodeType
        self.name = Name
        self.parent = Parent
        self.seq = GeneticSequence
        self.weight = Weight
        self.children = []

    def add_child(self, child):
        self.children.append(child)
        return child

    def length_tree(self):
        return len(self.children)


class Octree(QtWidgets.QMainWindow):
    def __init__(self, data_manager):
        super().__init__()
        self.setWindowTitle("Octree (Genetic Families)")
        self.data_manager = data_manager
        self.setFixedSize(800, 600)
        self.root_node = None
        self.tree = {}

        self.main_widget = QtWidgets.QWidget()
        self.setCentralWidget(self.main_widget)
        self.layout = QtWidgets.QVBoxLayout()
        self.main_widget.setLayout(self.layout)

        # Create a pyqtgraph PlotWidget and hide its axes
        self.plot_widget = pg.PlotWidget()
        self.plot_widget.hideAxis('bottom')
        self.plot_widget.hideAxis('left')
        self.layout.addWidget(self.plot_widget)

        self.node_positions = {}
        self.node_items = {}
        self.edge_items = []
        self.text_items = {}

        self.root_node = self.build_tree()
        self.layout_tree(self.root_node)
        self.create_visualization()
        self.center_on_screen()

        toolbar = QtWidgets.QToolBar("Main Toolbar")
        toolbar.setStyleSheet("background-color: rgb(100, 150, 255); color: black;")

        container = QtWidgets.QWidget()
        h_layout = QtWidgets.QHBoxLayout()
        container.setLayout(h_layout)

        left_spacer = QtWidgets.QWidget()
        left_spacer.setSizePolicy(QtWidgets.QSizePolicy.Policy.Expanding, QtWidgets.QSizePolicy.Policy.Preferred)
        h_layout.addWidget(left_spacer)

        add_species_act = QtGui.QAction("Add New", self)
        add_species_act.triggered.connect(self.add_species)
        button = QtWidgets.QPushButton("Add New")
        button.clicked.connect(lambda: add_species_act.trigger())
        h_layout.addWidget(button)

        right_spacer = QtWidgets.QWidget()
        right_spacer.setSizePolicy(QtWidgets.QSizePolicy.Policy.Expanding, QtWidgets.QSizePolicy.Policy.Preferred)
        h_layout.addWidget(right_spacer)

        toolbar.addWidget(container)
        self.addToolBar(toolbar)

    def build_tree(self):
        # Use C++ backend to get family records
        family_records = self.data_manager.get_family_dict()
        families = {}

        for rec in family_records:
            families[rec.Name.lower()] = Node('Family', rec.Name, rec.Parent)

        if 'sentinel' not in families:
            QtWidgets.QMessageBox.critical(self, "Error", "Sentinel not found in data!")
            return None

        self.root_node = families['sentinel']

        # Set up parent-child relationships among family nodes
        for rec in family_records:
            name = rec.Name.lower()
            if name == 'sentinel':
                continue
            parent_name = rec.Parent.lower() if rec.Parent else "sentinel"
            if parent_name in families:
                families[parent_name].add_child(families[name])

        return self.root_node

    def layout_tree(self, node, x=0, y=0, level=0, horizontal_spacing=100, vertical_spacing=80):
        self.node_positions[node] = (x, y)
        if not node.children:
            return x + horizontal_spacing
        next_x = x
        for child in node.children:
            next_x = self.layout_tree(child, next_x, y + vertical_spacing, level + 1, horizontal_spacing, vertical_spacing)
        # Center the parent over its children
        first_child_x = self.node_positions[node.children[0]][0]
        last_child_x = self.node_positions[node.children[-1]][0]
        self.node_positions[node] = ((first_child_x + last_child_x) / 2, y)
        return next_x

    def create_visualization(self):
        self.plot_widget.clear()
        self.node_items = {}
        self.edge_items = []
        self.text_items = {}

        edge_pen = pg.mkPen(color=(200, 200, 200), width=1.5)

        for node, (x, y) in self.node_positions.items():
            node_brush = pg.mkBrush(color=(100, 150, 255))
            scatter = pg.ScatterPlotItem()
            scatter.addPoints([x], [-y], size=10, brush=node_brush, pen=None)
            self.plot_widget.addItem(scatter)
            self.node_items[node] = scatter

            # Attach left-click event; ignore right-click events
            scatter.mousePressEvent = lambda event, n=node: self.family_node_clicked(event, n)

            text = pg.TextItem(node.name, anchor=(0.5, 0))
            text.setPos(x, -y - 5)
            self.plot_widget.addItem(text)
            self.text_items[node] = text

            for child in node.children:
                child_x, child_y = self.node_positions[child]
                line = pg.PlotCurveItem([x, child_x], [-y, -child_y], pen=edge_pen)
                self.plot_widget.addItem(line)
                self.edge_items.append(line)

        self.plot_widget.autoRange()

    def center_on_screen(self):
        screen_geometry = QtWidgets.QApplication.primaryScreen().availableGeometry()
        window_geometry = self.frameGeometry()
        center_point = screen_geometry.center()
        window_geometry.moveCenter(center_point)
        self.move(window_geometry.topLeft())

    def moveEvent(self, event):
        self.center_on_screen()

    def draw_octree(self):
        self.node_positions.clear()
        self.build_tree()
        self.layout_tree(self.root_node)
        self.create_visualization()

    def family_node_clicked(self, event, node):
        if event.button() == QtCore.Qt.MouseButton.LeftButton and node.name.lower() != 'sentinel':
            msgBox = QtWidgets.QMessageBox(self)
            msgBox.setIcon(QtWidgets.QMessageBox.Icon.Information)
            msgBox.setWindowTitle("Family Options")
            msgBox.setText(f"Select an action for family '{node.name}'.")
            delete_button = msgBox.addButton("Delete Family", QtWidgets.QMessageBox.ButtonRole.ActionRole)
            view_button = msgBox.addButton("View BK-tree", QtWidgets.QMessageBox.ButtonRole.ActionRole)
            msgBox.addButton("Close", QtWidgets.QMessageBox.ButtonRole.RejectRole)

            rect = msgBox.frameGeometry()
            rect.moveCenter(self.geometry().center())
            msgBox.move(rect.topLeft())
            msgBox.exec()
            clicked = msgBox.clickedButton()

            if clicked == delete_button:
                reply = QtWidgets.QMessageBox.question(
                    self, "Confirm Delete",
                    f"Delete family '{node.name}'?",
                    QtWidgets.QMessageBox.StandardButton.Yes | QtWidgets.QMessageBox.StandardButton.No,
                    QtWidgets.QMessageBox.StandardButton.No
                )
                if reply == QtWidgets.QMessageBox.StandardButton.Yes:
                    # Use C++ backend to delete family
                    result = self.data_manager.delete_family(node.name)
                    if result:
                        QtWidgets.QMessageBox.information(self, "Info", "Family deleted. Tree updated.")
                        self.draw_octree()
                    else:
                        QtWidgets.QMessageBox.warning(self, "Warning", "Failed to delete family.")
            elif clicked == view_button:
                self.open_bk_tree(node.name)

    def open_bk_tree(self, family_name):
        self.bk_window = BKTree(self.data_manager, family_name)
        self.bk_window.show()
        self.close()

    def add_species(self):
        while True:
            seq, ok = QtWidgets.QInputDialog.getText(self, "Input Genetic Sequence",
                                                    "Enter 128-character genetic sequence:")
            if not ok:
                return  # canceled
            seq = seq.strip()
            if len(seq) != 128:
                QtWidgets.QMessageBox.warning(self, "Error", "Genetic sequence must be exactly 128 characters.")
                continue
            # Use C++ backend to check for duplicate sequence
            existing = self.data_manager.find_by_genetic_sequence(seq)
            if existing.Name:  # If name exists, record exists
                QtWidgets.QMessageBox.warning(self, "Error", "Species already exists (sequence duplicate).")
                return
            break

        while True:
            family, ok = QtWidgets.QInputDialog.getText(self, "Input Family Name", "Enter family name:")
            if not ok:
                return
            family = family.title().strip()
            if not family:
                QtWidgets.QMessageBox.warning(self, "Error", "Family name cannot be empty.")
                continue
            # Use C++ backend to check family list
            family_list = self.data_manager.get_family_list()
            if family in family_list:
                QtWidgets.QMessageBox.warning(self, "Error", "Family already exists.")
                return
            break

        while True:
            parent_family, ok = QtWidgets.QInputDialog.getText(
                self, "Input Parent Family",
                "Enter parent's family name for this family (leave blank for Sentinel):"
            )
            if not ok:
                return
            parent_family = parent_family.title().strip()
            if not parent_family:
                parent_family = "Sentinel"
            else:
                # Use C++ backend to check if parent family exists
                family_list = self.data_manager.get_family_list()
                if parent_family not in family_list:
                    QtWidgets.QMessageBox.warning(self, "Error", "Family Does Not Exist.")
                    return
            break

        # Create and save the new family using C++ backend
        family_record = backend_bridge.Record()
        family_record.NodeType = "Family"
        family_record.Name = family
        family_record.Parent = parent_family
        family_record.GeneticSequence = ""
        family_record.Weight = 1
        self.data_manager.save_in_memory_record(family_record)

        while True:
            specie, ok = QtWidgets.QInputDialog.getText(self, "Input Species Name", "Enter species name:")
            if not ok:
                return
            specie = specie.title().strip()
            if not specie:
                QtWidgets.QMessageBox.warning(self, "Error", "Species name cannot be empty.")
                continue
            # Use C++ backend to check for species name
            if self.data_manager.name_exists_with_diff_sequence("Species", specie, seq):
                QtWidgets.QMessageBox.warning(self, "Error", "Species name already exists for a different sequence.")
                return
            break

        # Create and save the new species using C++ backend
        species_record = backend_bridge.Record()
        species_record.NodeType = "Species"
        species_record.Name = specie
        species_record.Parent = family
        species_record.GeneticSequence = seq
        species_record.Weight = 1  # default weight for demonstration
        self.data_manager.save_in_memory_record(species_record)
        
        QtWidgets.QMessageBox.information(self, "Info", "Species added. Tree updated.")
        self.draw_octree()


class BKTree(QtWidgets.QMainWindow):
    def __init__(self, data_manager, family_name):
        super().__init__()
        self.data_manager = data_manager
        self.family_name = family_name.title().strip()
        self.setWindowTitle(f"BK-Tree for {self.family_name}")
        self.setFixedSize(800, 600)

        self.main_widget = QtWidgets.QWidget()
        self.setCentralWidget(self.main_widget)
        self.layout = QtWidgets.QVBoxLayout()
        self.main_widget.setLayout(self.layout)

        toolbar = QtWidgets.QToolBar("Back")
        toolbar.setStyleSheet("background-color: rgb(100, 150, 255); color: black;")
        container = QtWidgets.QWidget()
        h_layout = QtWidgets.QHBoxLayout()
        container.setLayout(h_layout)
        left_spacer = QtWidgets.QWidget()
        left_spacer.setSizePolicy(QtWidgets.QSizePolicy.Policy.Expanding, QtWidgets.QSizePolicy.Policy.Preferred)
        h_layout.addWidget(left_spacer)
        back_act = QtGui.QAction("Back Button", self)
        back_act.triggered.connect(self.return_to_octree)
        button = QtWidgets.QPushButton("Back")
        button.clicked.connect(lambda: back_act.trigger())
        h_layout.addWidget(button)
        right_spacer = QtWidgets.QWidget()
        right_spacer.setSizePolicy(QtWidgets.QSizePolicy.Policy.Expanding, QtWidgets.QSizePolicy.Policy.Preferred)
        h_layout.addWidget(right_spacer)
        toolbar.addWidget(container)
        self.addToolBar(toolbar)

        self.plot_widget = pg.PlotWidget()
        self.plot_widget.hideAxis('bottom')
        self.plot_widget.hideAxis('left')
        self.layout.addWidget(self.plot_widget)

        self.node_positions = {}
        self.node_items = {}
        self.edge_items = []
        self.text_items = {}

        self.root_node = self.build_tree()
        self.layout_tree(self.root_node)
        self.create_visualization()
        self.center_on_screen()

    def build_tree(self):
        root = Node("Family", self.family_name)
        # Use C++ backend to get species by family
        species_records = self.data_manager.get_species_by_family(self.family_name)
        for rec in species_records:
            species_node = Node("Species", rec.Name, rec.Parent, rec.GeneticSequence, rec.Weight)
            root.add_child(species_node)
        return root

    def layout_tree(self, node, x=0, y=0, level=0, horizontal_spacing=100, vertical_spacing=80):
        self.node_positions[node] = (x, y)
        if not node.children:
            return x + horizontal_spacing
        next_x = x
        for child in node.children:
            next_x = self.layout_tree(child, next_x, y + vertical_spacing, level + 1, horizontal_spacing, vertical_spacing)
        first_child_x = self.node_positions[node.children[0]][0]
        last_child_x = self.node_positions[node.children[-1]][0]
        self.node_positions[node] = ((first_child_x + last_child_x) / 2, y)
        return next_x

    def create_visualization(self):
        self.plot_widget.clear()
        self.node_items = {}
        self.edge_items = []
        self.text_items = {}

        edge_pen = pg.mkPen(color=(200, 200, 200), width=1.5)

        for node, (x, y) in self.node_positions.items():
            if node == self.root_node:
                node_brush = pg.mkBrush(color=(255, 100, 100))  # Red for the root.
            else:
                node_brush = pg.mkBrush(color=(100, 200, 100))  # Green for species.
            scatter = pg.ScatterPlotItem()
            scatter.addPoints([x], [-y], size=10, brush=node_brush, pen=None)
            self.plot_widget.addItem(scatter)
            self.node_items[node] = scatter

            scatter.mousePressEvent = lambda event, n=node: self.species_node_clicked(event, n)

            display_text = node.name
            if node != self.root_node:
                display_text += f"\n({node.weight})"
            text_item = pg.TextItem(display_text, anchor=(0.5, 0))
            text_item.setPos(x, -y - 5)
            self.plot_widget.addItem(text_item)
            self.text_items[node] = text_item

            for child in node.children:
                child_x, child_y = self.node_positions[child]
                line = pg.PlotCurveItem([x, child_x], [-y, -child_y], pen=edge_pen)
                self.plot_widget.addItem(line)
                self.edge_items.append(line)

        self.plot_widget.autoRange()

    def center_on_screen(self):
        screen_geometry = QtWidgets.QApplication.primaryScreen().availableGeometry()
        window_geometry = self.frameGeometry()
        center_point = screen_geometry.center()
        window_geometry.moveCenter(center_point)
        self.move(window_geometry.topLeft())

    def species_node_clicked(self, event, node):
        print(f"Species node clicked: {node.name}")
        # Handle species node click event
    def species_node_clicked(self, event, node):
        print(f"Species node clicked: {node.name}")
        
        # Only handle clicks on species nodes (ignore root and family nodes)
        if node.type != 'Species':
            return
        
        # Display a message box with options to view or edit the species
        msgBox = QtWidgets.QMessageBox(self)
        msgBox.setIcon(QtWidgets.QMessageBox.Icon.Information)
        msgBox.setWindowTitle(f"Species Options - {node.name}")
        msgBox.setText(f"Select an action for species '{node.name}'.")

        # Add buttons for different actions
        delete_button = msgBox.addButton("Delete Species", QtWidgets.QMessageBox.ButtonRole.ActionRole)
        view_button = msgBox.addButton("View Genetic Sequence", QtWidgets.QMessageBox.ButtonRole.ActionRole)
        edit_button = msgBox.addButton("Edit Species", QtWidgets.QMessageBox.ButtonRole.ActionRole)
        msgBox.addButton("Close", QtWidgets.QMessageBox.ButtonRole.RejectRole)

        # Center the message box on the window
        rect = msgBox.frameGeometry()
        rect.moveCenter(self.geometry().center())
        msgBox.move(rect.topLeft())

        # Show the message box and wait for user selection
        msgBox.exec()
        
        clicked_button = msgBox.clickedButton()

        if clicked_button == delete_button:
            # Ask for confirmation before deletion
            confirm_reply = QtWidgets.QMessageBox.question(
                self, "Confirm Deletion",
                f"Are you sure you want to delete species '{node.name}'?",
                QtWidgets.QMessageBox.StandardButton.Yes | QtWidgets.QMessageBox.StandardButton.No,
                QtWidgets.QMessageBox.StandardButton.No
            )
            
            if confirm_reply == QtWidgets.QMessageBox.StandardButton.Yes:
                # Use the C++ backend to delete the species
                result = self.data_manager.delete_species(node.name)
                if result:
                    QtWidgets.QMessageBox.information(self, "Species Deleted", f"Species '{node.name}' deleted successfully.")
                    self.draw_octree()  # Refresh the tree view
                else:
                    QtWidgets.QMessageBox.warning(self, "Error", f"Failed to delete species '{node.name}'.")
        
        elif clicked_button == view_button:
            # Display the genetic sequence in a dialog box
            QtWidgets.QMessageBox.information(self, f"Genetic Sequence of {node.name}", f"Genetic Sequence: {node.seq}")
        
        elif clicked_button == edit_button:
            # Allow the user to edit the species' name or genetic sequence
            new_name, ok = QtWidgets.QInputDialog.getText(self, "Edit Species", "Enter new name:", text=node.name)
            if ok and new_name.strip():
                # Use the C++ backend to check if the name already exists
                if self.data_manager.name_exists(new_name.strip()):
                    QtWidgets.QMessageBox.warning(self, "Error", "Species name already exists.")
                    return
                node.name = new_name.strip()  # Update the species name
                self.data_manager.update_species_name(node.name, new_name.strip())  # Save change in backend
                QtWidgets.QMessageBox.information(self, "Species Edited", f"Species '{node.name}' updated successfully.")
                self.draw_octree()  # Refresh the tree view
