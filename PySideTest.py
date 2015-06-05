#!/usr/bin/python
 
# Import PySide classes
import sys
from PySide.QtCore import *
from PySide.QtGui import *
import FabricUI

app = QApplication(sys.argv)
mainWindow = QMainWindow()
layout = QVBoxLayout()
dfgWidget = FabricUI.FabricUI.DFG.DFGWidget()
layout.addWidget(dfgWidget)
mainWindow.setLayout(layout)
mainWindow.show()
app.exec_()
sys.exit()
