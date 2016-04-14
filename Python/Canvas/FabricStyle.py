from FabricEngine.FabricUI import Style
from PySide import QtGui

# [andrew 20151028] shiboken thinks FabricStyle is an abstract class
# unless we re-define the virtual standardPixmap method
class FabricStyle(Style.FabricStyle):
    def __init__(self):
        super(FabricStyle, self).__init__()

    def standardPixmap(self, _standardPixmap, _option=None, _widget=None):
        return QtGui.QPixmap()

