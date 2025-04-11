import sys
import csv
from PyQt6 import QtWidgets, QtGui, QtCore
import pyqtgraph as pg

class DataManager:
    def __init__(self, csv_file):
        self.csv_file = csv_file
        self.records = []  # each record is a dict loaded from CSV
        self.load_csv()

    def load_csv(self):
        self.records.clear()
        with open(self.csv_file, "r", newline="") as f:
            reader = csv.DictReader(f)
            for row in reader:
                row["Name"] = row["Name"].title().strip()
                row["Parent"] = row["Parent"].title().strip() if row["Parent"] else ""
                row["Weight"] = float(row["Weight"]) if row["Weight"] else 0.0
                self.records.append(row)
        f.close()

    def save_in_memory_record(self, record):
        self.records.append(record)

    def delete_family(self, family_name):
        if family_name.lower() == "sentinel":
            return False
        # Remove all records corresponding to that family (and species that belong to it)
        family_name = family_name.title()
        self.records = [r for r in self.records if not (
            (r["NodeType"] == "Family" and r["Name"] == family_name)
            or (r["NodeType"] == "Species" and r["Parent"] == family_name)
        )]

    def find_by_genetic_sequence(self, sequence):
        for record in self.records:
            if record["NodeType"] == "Species" and record["GeneticSequence"] == sequence:
                return record
        return None

    def name_exists_with_diff_sequence(self, node_type, name, sequence):
        # Check if any record with given name exists with a different genetic sequence.
        for record in self.records:
            if record["Name"].lower() == name.lower():
                if node_type == "Species":
                    # If the found record's sequence is different from the input sequence.
                    if record["GeneticSequence"] != sequence:
                        return True
                else:
                    return True
        return False

    def get_families(self):
        # Return a list of family records.
        return [r for r in self.records if r["NodeType"] == "Family"]

    def get_species_by_family(self, family_name):
        # Return species records whose parent is the given family.
        return [r for r in self.records if r["NodeType"] == "Species" and r["Parent"] == family_name.title()]

class Node:
    def __init__(self, NodeType, Name, Parent = None, GeneticSequence = None, Weight = None):
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

# Octree class (visualizes genetic families)
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

        # Data for visualization
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
        records = self.data_manager.records
        families = {}
        for rec in records:
            if rec['NodeType'] == 'Family':
                families[rec['Name'].lower()] = Node('Family', rec['Name'], rec['Parent'])
        if 'sentinel' not in families:
            QtWidgets.QMessageBox.critical(self, "Error", "Sentinel not found in data!")
            return None
        self.root_node = families['sentinel']

        # Set up parent-child relationships among family nodes.
        for rec in records:
            if rec['NodeType'] == 'Family':
                name = rec['Name'].lower()
                if name == 'sentinel':
                    continue
                parent_name = rec['Parent'].lower() if rec['Parent'] else "sentinel"
                if parent_name in families:
                    families[parent_name].add_child(families[name])
        
        return self.root_node

    def layout_tree(self, node, x=0, y=0, level=0, horizontal_spacing=100, vertical_spacing=80):
        self.node_positions[node] = (x, y)
        if not node.children:
            return x + horizontal_spacing
        next_x = x
        for child in node.children:
            next_x = self.layout_tree(child, next_x, y + vertical_spacing, level + 1, 
                                       horizontal_spacing, vertical_spacing)
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
                    self.data_manager.delete_family(node.name)
                    QtWidgets.QMessageBox.information(self, "Info", "Family deleted. Tree updated.")
                    self.draw_octree()
            elif clicked == view_button:
                self.open_bk_tree(node.name)

    def open_bk_tree(self, family_name):
        self.bk_window = BKTreeWindow(self.data_manager, family_name)
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
            if self.data_manager.find_by_genetic_sequence(seq):
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
            break

        families = self.data_manager.get_families()
        if not any(rec["Name"].lower() == family.lower() for rec in families):
            new_family_record = {
                "NodeType": "Family",
                "Name": family,
                "Parent": parent_family,
                "GeneticSequence": "",
                "Weight": 0.0
            }
            self.data_manager.save_in_memory_record(new_family_record)

        while True:
            specie, ok = QtWidgets.QInputDialog.getText(self, "Input Species Name", "Enter species name:")
            if not ok:
                return
            specie = specie.title().strip()
            if not specie:
                QtWidgets.QMessageBox.warning(self, "Error", "Species name cannot be empty.")
                continue
            if self.data_manager.name_exists_with_diff_sequence("Species", specie, seq):
                QtWidgets.QMessageBox.warning(self, "Error", "Species name already exists for a different sequence.")
                return
            break

        new_record = {
            "NodeType": "Species",
            "Name": specie,
            "Parent": family,
            "GeneticSequence": seq,
            "Weight": 1.0  # default weight for demonstration
        }
        self.data_manager.save_in_memory_record(new_record)
        QtWidgets.QMessageBox.information(self, "Info", "Species added. Tree updated.")
        self.draw_octree()
  

class BKTreeWindow(QtWidgets.QMainWindow):
    def __init__(self, data_manager, family_name):
        super().__init__()
        self.setWindowTitle(f"BK-Tree for {family_name}")
        self.data_manager = data_manager
        self.family_name = family_name
        self.setFixedSize(800, 600)

        central = QtWidgets.QWidget()
        self.setCentralWidget(central)
        vbox = QtWidgets.QVBoxLayout(central)

        # Back button at top.
        back_button = QtWidgets.QPushButton("Back")
        back_button.clicked.connect(self.return_to_octree)
        vbox.addWidget(back_button)

        self.scene = QtWidgets.QGraphicsScene(self)
        self.view = QtWidgets.QGraphicsView(self.scene, self)
        vbox.addWidget(self.view)

        self.draw_bk_tree()

    def draw_bk_tree(self):
        self.scene.clear()
        species_records = self.data_manager.get_species_by_family(self.family_name)
        if not species_records:
            QtWidgets.QMessageBox.information(self, "Info", f"No species found for family {self.family_name}.")
            return

        # Build a simple vertical layout: assume each species (first record is root) gets its own column.
        y = 50
        x = 100
        gap_y = 100
        gap_x = 200

        # For demonstration, we assume each species name appears only once.
        for idx, record in enumerate(species_records):
            specie_item = NodeItem(x + idx * gap_x, y, 40, record["Name"], record)
            self.scene.addItem(specie_item)
            specie_item.setToolTip("Species: " + record["Name"])
            specie_item.mousePressEvent = lambda event, item=specie_item: self.specie_node_clicked(event, item)

    def specie_node_clicked(self, event, node_item):
        if event.button() == QtCore.Qt.MouseButton.LeftButton:
            reply = QtWidgets.QMessageBox.question(self, "Confirm Mutation",
                                                   "Confirm Markov model mutation for this species?",
                                                   QtWidgets.QMessageBox.StandardButton.Yes |
                                                   QtWidgets.QMessageBox.StandardButton.No)
            if reply == QtWidgets.QMessageBox.StandardButton.Yes:
                # Here you would call your mutation algorithm.
                # For now, we simulate by changing node color.
                node_item.setBrush(QtGui.QBrush(QtCore.Qt.GlobalColor.green))
                QtWidgets.QMessageBox.information(self, "Info", "Mutated successfully.")
                # Return to octree view after mutation.
                self.return_to_octree()

    def return_to_octree(self):
        self.octree_window = OctreeWindow(self.data_manager)
        self.octree_window.show()
        self.close()

def main():
    app = QtWidgets.QApplication(sys.argv)
    data_manager = DataManager("data.csv")
    window = Octree(data_manager)
    window.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
