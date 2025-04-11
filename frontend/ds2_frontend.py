import sys
import random
import math
import numpy as np
from PyQt6 import QtWidgets, QtCore, QtGui
import pyqtgraph as pg

# ------------------------------
# Backend: Taxonomic Tree Data Structure
# ------------------------------

class TaxonomyNode:
    def __init__(self, name, rank, children=None):
        self.name = name
        self.rank = rank  # e.g., "kingdom", "phylum", "class", etc.
        self.children = children if children is not None else []
    
    def add_child(self, child):
        self.children.append(child)
        return child

def generate_sample_taxonomy():
    """Generate a sample taxonomic tree for visualization"""
    # Root
    root = TaxonomyNode("Life", "domain")
    
    # Kingdoms
    animals = root.add_child(TaxonomyNode("Animalia", "kingdom"))
    plants = root.add_child(TaxonomyNode("Plantae", "kingdom"))
    fungi = root.add_child(TaxonomyNode("Fungi", "kingdom"))
    
    # Animal phyla
    chordates = animals.add_child(TaxonomyNode("Chordata", "phylum"))
    arthropods = animals.add_child(TaxonomyNode("Arthropoda", "phylum"))
    
    # Chordate classes
    mammals = chordates.add_child(TaxonomyNode("Mammalia", "class"))
    birds = chordates.add_child(TaxonomyNode("Aves", "class"))
    reptiles = chordates.add_child(TaxonomyNode("Reptilia", "class"))
    
    # Mammal orders
    primates = mammals.add_child(TaxonomyNode("Primates", "order"))
    carnivores = mammals.add_child(TaxonomyNode("Carnivora", "order"))
    
    # Primate families
    hominids = primates.add_child(TaxonomyNode("Hominidae", "family"))
    
    # Hominid genera
    homo = hominids.add_child(TaxonomyNode("Homo", "genus"))
    pan = hominids.add_child(TaxonomyNode("Pan", "genus"))
    gorilla = hominids.add_child(TaxonomyNode("Gorilla", "genus"))
    
    # Homo species
    homo.add_child(TaxonomyNode("Homo sapiens", "species"))
    homo.add_child(TaxonomyNode("Homo neanderthalensis", "species"))
    
    # Pan species
    pan.add_child(TaxonomyNode("Pan troglodytes", "species"))
    pan.add_child(TaxonomyNode("Pan paniscus", "species"))
    
    # Add more branches as needed...
    
    return root

def search_taxonomy(node, target_name, path=None):
    """Search for a node by name, recording the path taken"""
    if path is None:
        path = []
    
    path.append(node)
    
    if node.name.lower() == target_name.lower():
        return True
    
    for child in node.children:
        if search_taxonomy(child, target_name, path):
            return True
    
    path.pop()
    return False

# ------------------------------
# Front End: Taxonomy Tree Visualizer
# ------------------------------

class TaxonomyTreeVisualizer(QtWidgets.QMainWindow):
    def __init__(self, root_node):
        super().__init__()
        self.root_node = root_node
        self.setWindowTitle("Taxonomy Tree Visualization")
        
        # Main layout
        self.main_widget = QtWidgets.QWidget()
        self.setCentralWidget(self.main_widget)
        self.layout = QtWidgets.QVBoxLayout()
        self.main_widget.setLayout(self.layout)
        
        # Search controls
        search_layout = QtWidgets.QHBoxLayout()
        self.search_input = QtWidgets.QLineEdit()
        self.search_input.setPlaceholderText("Enter taxon name to search")
        self.search_button = QtWidgets.QPushButton("Search")
        self.search_button.clicked.connect(self.start_search)
        search_layout.addWidget(self.search_input)
        search_layout.addWidget(self.search_button)
        self.layout.addLayout(search_layout)
        
        # Graph widget
        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setBackground('k')  # Black background
        self.plot_widget.setAspectLocked(True)  # Keep aspect ratio
        self.layout.addWidget(self.plot_widget)
        
        # Create tree layout and visualization
        self.node_positions = {}
        self.node_items = {}
        self.edge_items = []
        self.text_items = {}
        self.layout_tree(root_node)
        self.create_visualization()
        
        # Animation variables
        self.search_path = []
        self.animation_timer = QtCore.QTimer()
        self.animation_timer.timeout.connect(self.animate_search_step)
        self.animation_index = 0
        self.zoom_animation = QtCore.QVariantAnimation()
        self.zoom_animation.valueChanged.connect(self.update_zoom)
        
        # Resize and show
        self.resize(900, 700)
    
    def layout_tree(self, node, x=0, y=0, level=0, horizontal_spacing=100, vertical_spacing=80):
        """Recursively compute positions for nodes in the tree"""
        self.node_positions[node] = (x, y)
        
        if not node.children:
            return x + horizontal_spacing
        
        # Compute positions for children
        next_x = x
        for child in node.children:
            next_x = self.layout_tree(child, next_x, y + vertical_spacing, level + 1, 
                                     horizontal_spacing, vertical_spacing)
        
        # Center parent over children
        first_child_x = self.node_positions[node.children[0]][0]
        last_child_x = self.node_positions[node.children[-1]][0]
        self.node_positions[node] = ((first_child_x + last_child_x) / 2, y)
        
        return next_x
    
    def create_visualization(self):
        """Create the visual representation of the tree"""
        # Clear any existing items
        self.plot_widget.clear()
        self.node_items = {}
        self.edge_items = []
        self.text_items = {}
        
        # Create a pen for edges
        edge_pen = pg.mkPen(color=(200, 200, 200), width=1.5)
        
        # Create nodes and edges
        for node, (x, y) in self.node_positions.items():
            # Create node as a scatter point
            node_brush = pg.mkBrush(color=(100, 150, 255))
            scatter = pg.ScatterPlotItem()
            scatter.addPoints([x], [-y], size=10, brush=node_brush, pen=None)
            self.plot_widget.addItem(scatter)
            self.node_items[node] = scatter
            
            # Create node label
            text = pg.TextItem(node.name, anchor=(0.5, 0))
            text.setPos(x, -y - 15)
            self.plot_widget.addItem(text)
            self.text_items[node] = text
            
            # Create edges to children
            for child in node.children:
                child_x, child_y = self.node_positions[child]
                line = pg.PlotCurveItem(
                    [x, child_x], [-y, -child_y], 
                    pen=edge_pen
                )
                self.plot_widget.addItem(line)
                self.edge_items.append(line)
        
        # Auto-range to show the entire tree
        self.plot_widget.autoRange()
    
    def start_search(self):
        """Start search animation for the requested taxon"""
        search_text = self.search_input.text().strip()
        if not search_text:
            return
        
        # Reset previous search state
        self.reset_visualization()
        
        # Perform search
        self.search_path = []
        found = search_taxonomy(self.root_node, search_text, self.search_path)
        
        if not found:
            QtWidgets.QMessageBox.information(self, "Search Result", 
                                            f"'{search_text}' not found in the taxonomy tree.")
            return
        
        # Start animation
        self.animation_index = 0
        self.animation_timer.start(800)  # 800ms per step
    
    def reset_visualization(self):
        """Reset visualization to original state"""
        self.animation_timer.stop()
        self.zoom_animation.stop()
        
        # Reset node colors
        for node, scatter in self.node_items.items():
            scatter.setBrush(pg.mkBrush(color=(100, 150, 255)))
        
        # Reset view
        self.plot_widget.autoRange()
    
    def animate_search_step(self):
        """Animate one step of the search path"""
        if self.animation_index >= len(self.search_path):
            self.animation_timer.stop()
            return
        
        # Get current node in the path
        current_node = self.search_path[self.animation_index]
        scatter = self.node_items[current_node]
        
        # Highlight the current node
        scatter.setBrush(pg.mkBrush(color=(255, 100, 100)))
        
        # If this is the final node (target found), zoom in on it
        if self.animation_index == len(self.search_path) - 1:
            self.zoom_to_node(current_node)
        
        self.animation_index += 1
    
    def zoom_to_node(self, node):
        """Animate zooming to a specific node"""
        # Get current view rect
        current_rect = self.plot_widget.viewRect()
        
        # Calculate target view rect (centered on node with some padding)
        x, y = self.node_positions[node]
        target_rect = QtCore.QRectF(x - 100, -y - 100, 200, 200)
        
        # Create animation
        self.zoom_animation.setStartValue(current_rect)
        self.zoom_animation.setEndValue(target_rect)
        self.zoom_animation.setDuration(1000)  # 1 second
        self.zoom_animation.start()
    
    def update_zoom(self, rect):
        """Update view during zoom animation"""
        self.plot_widget.setRange(rect, padding=0)

# ------------------------------
# Main Application
# ------------------------------

def main():
    app = QtWidgets.QApplication(sys.argv)
    
    # Generate sample data
    root = generate_sample_taxonomy()
    
    # Create and show visualization
    window = TaxonomyTreeVisualizer(root)
    window.show()
    
    sys.exit(app.exec())

if __name__ == '__main__':
    main()