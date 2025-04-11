import sys
import csv
from PyQt6 import QtWidgets, QtGui, QtCore

# =======================
# Data Manager
# =======================
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
                # Normalize names: title case for Name and Parent
                row["Name"] = row["Name"].title().strip()
                row["Parent"] = row["Parent"].title().strip() if row["Parent"] else ""
                # Store weight as float if provided, else 0
                row["Weight"] = float(row["Weight"]) if row["Weight"] else 0.0
                self.records.append(row)

    def save_in_memory_record(self, record):
        self.records.append(record)
        # In a live system, you might write to CSV. For now, changes are ephemeral.

    def delete_family(self, family_name):
        # Remove all records corresponding to that family (and species that belong to it)
        family_name = family_name.title()
        self.records = [r for r in self.records if not (
            (r["NodeType"] == "Family" and r["Name"] == family_name)
            or (r["NodeType"] == "Species" and r["Parent"] == family_name)
        )]

    def find_by_genetic_sequence(self, sequence):
        # Compare genetic sequences only among species records
        for record in self.records:
            if record["NodeType"] == "Species" and record["GeneticSequence"] == sequence:
                return record
        return None

    def name_exists_with_diff_sequence(self, node_type, name, sequence):
        # Check if any record with given name exists with a different genetic sequence.
        # For families, sequence is not applicable.
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

# =======================
# Graphics Items for Nodes
# =======================
class NodeItem(QtWidgets.QGraphicsEllipseItem):
    """Custom graphics item representing a node in the tree."""
    def __init__(self, x, y, diameter, label, node_data, parent=None):
        super().__init__(0, 0, diameter, diameter, parent)
        self.setPos(x, y)
        self.setBrush(QtGui.QBrush(QtCore.Qt.GlobalColor.lightGray))
        self.setPen(QtGui.QPen(QtCore.Qt.GlobalColor.black))
        self.label = label
        self.node_data = node_data
        # Enable mouse events
        self.setFlag(QtWidgets.QGraphicsItem.GraphicsItemFlag.ItemIsSelectable, True)

    def paint(self, painter, option, widget=None):
        super().paint(painter, option, widget)
        # Draw label in the center
        rect = self.rect()
        painter.drawText(rect, QtCore.Qt.AlignmentFlag.AlignCenter, self.label)

# =======================
# Octree (Family Tree) Window
# =======================
class OctreeWindow(QtWidgets.QMainWindow):
    def __init__(self, data_manager):
        super().__init__()
        self.setWindowTitle("Octree (Family Tree)")
        self.data_manager = data_manager
        self.setFixedSize(800, 600)

        # Graphics Scene and View
        self.scene = QtWidgets.QGraphicsScene(self)
        self.view = QtWidgets.QGraphicsView(self.scene, self)
        self.setCentralWidget(self.view)

        # Toolbar for adding species
        toolbar = QtWidgets.QToolBar("Main Toolbar")
        self.addToolBar(toolbar)
        add_species_act = QtGui.QAction("Add Species", self)
        add_species_act.triggered.connect(self.add_species)
        toolbar.addAction(add_species_act)

        self.draw_octree()

    def draw_octree(self):
        self.scene.clear()
        families = self.data_manager.get_families()

        # Build a dictionary for quick lookup and for our (simple) tree structure.
        # For this demo, assume the sentinel node is always named "Sentinel"
        sentinel = None
        for fam in families:
            if fam["Name"].lower() == "sentinel":
                sentinel = fam
                break

        if not sentinel:
            QtWidgets.QMessageBox.critical(self, "Error", "Sentinel node not found in data.")
            return

        # For demonstration, we draw the sentinel in the center and arrange family nodes radially.
        center_x, center_y = 400, 300
        sentinel_item = NodeItem(center_x - 20, center_y - 20, 40, sentinel["Name"], sentinel)
        self.scene.addItem(sentinel_item)
        sentinel_item.setFlag(QtWidgets.QGraphicsItem.GraphicsItemFlag.ItemIsSelectable, False)

        # Draw the first level: families whose parent is Sentinel
        first_level = [fam for fam in families if fam["Parent"].lower() == "sentinel"]
        radius = 150
        angle_step = 360 / len(first_level) if first_level else 0

        for idx, fam in enumerate(first_level):
            angle = angle_step * idx
            rad = angle * 3.14 / 180
            x = center_x + radius * QtCore.qCos(rad) - 20
            y = center_y + radius * QtCore.qSin(rad) - 20
            fam_item = NodeItem(x, y, 40, fam["Name"], fam)
            self.scene.addItem(fam_item)
            fam_item.setToolTip("Family: " + fam["Name"])
            # Install event filter or override mousePressEvent:
            fam_item.mousePressEvent = lambda event, item=fam_item: self.family_node_clicked(event, item)

        # For further levels we could compute positions relative to their parent.
        # For simplicity, here we assume no more than one level after sentinel.

    def family_node_clicked(self, event, node_item):
        if event.button() == QtCore.Qt.MouseButton.LeftButton:
            # Create a pop-up menu with options: Delete Family, View BK-tree
            menu = QtWidgets.QMenu()
            delete_action = menu.addAction("Delete Family")
            view_bk_action = menu.addAction("View BK-tree")
            action = menu.exec(self.mapToGlobal(event.pos().toPoint()))
            if action == delete_action:
                reply = QtWidgets.QMessageBox.question(self, "Confirm Delete",
                                                       f"Delete family '{node_item.label}'?",
                                                       QtWidgets.QMessageBox.StandardButton.Yes |
                                                       QtWidgets.QMessageBox.StandardButton.No)
                if reply == QtWidgets.QMessageBox.StandardButton.Yes:
                    self.data_manager.delete_family(node_item.label)
                    QtWidgets.QMessageBox.information(self, "Info", "Family deleted. Tree updated.")
                    self.draw_octree()
            elif action == view_bk_action:
                # Open BK-tree view for the selected family.
                self.open_bk_tree(node_item.label)

    def open_bk_tree(self, family_name):
        self.bk_window = BKTreeWindow(self.data_manager, family_name)
        self.bk_window.show()
        self.close()

    def add_species(self):
        # Sequential input dialogs: genetic sequence -> species name -> family name.
        # Genetic Sequence
        while True:
            seq, ok = QtWidgets.QInputDialog.getText(self, "Input Genetic Sequence",
                                                       "Enter 128-character genetic sequence:")
            if not ok:
                return  # canceled
            seq = seq.strip()
            if len(seq) != 128:
                QtWidgets.QMessageBox.warning(self, "Error", "Genetic sequence must be exactly 128 characters.")
                continue
            # Check if sequence already exists
            if self.data_manager.find_by_genetic_sequence(seq):
                QtWidgets.QMessageBox.warning(self, "Error", "Species already exists (sequence duplicate).")
                return
            break

        # Species Name
        while True:
            specie, ok = QtWidgets.QInputDialog.getText(self, "Input Species Name", "Enter species name:")
            if not ok:
                return
            specie = specie.title().strip()
            if not specie:
                QtWidgets.QMessageBox.warning(self, "Error", "Species name cannot be empty.")
                continue
            # Check if name exists with a different sequence
            if self.data_manager.name_exists_with_diff_sequence("Species", specie, seq):
                QtWidgets.QMessageBox.warning(self, "Error", "Species name already exists for a different sequence.")
                return
            break

        # Family Name
        while True:
            family, ok = QtWidgets.QInputDialog.getText(self, "Input Family Name", "Enter family name:")
            if not ok:
                return
            family = family.title().strip()
            if not family:
                QtWidgets.QMessageBox.warning(self, "Error", "Family name cannot be empty.")
                continue
            # Check if family exists with a different record (for species, it’s an error if name exists with different sequence)
            if self.data_manager.name_exists_with_diff_sequence("Species", family, seq):
                QtWidgets.QMessageBox.warning(self, "Error", "Family name already exists for a different sequence.")
                return
            break

        # If checks pass, create a new species record.
        new_record = {
            "NodeType": "Species",
            "Name": specie,
            "Parent": family,
            "GeneticSequence": seq,
            "Weight": 1.0  # for demonstration, set a default weight
        }
        self.data_manager.save_in_memory_record(new_record)
        QtWidgets.QMessageBox.information(self, "Info", "Species added. Tree updated.")
        self.draw_octree()

# =======================
# BK-Tree (Species Tree) Window
# =======================
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

# =======================
# Main Application Runner
# =======================
def main():
    app = QtWidgets.QApplication(sys.argv)
    data_manager = DataManager("data.csv")
    window = OctreeWindow(data_manager)
    window.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
