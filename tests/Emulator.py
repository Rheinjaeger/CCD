from PyQt5 import QtWidgets, QtGui, QtCore
import sys

class CCDControlGUI(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
    
    def initUI(self):
        self.setWindowTitle("Space CCD Sensor Control")
        self.setGeometry(100, 100, 800, 600)

        #Layouts
        main_layout = QtWidgets.QVBoxLayout()
        command_layout = QtWidgets.QHBoxLayout()
        image_layout = QtWidgets.QVBoxLayout()
        log_layout = QtWidgets.QVBoxLayout()
        
        #Sending commands
        self.command_input = QtWidgets.QLineEdit()
        self.command_input.setPlaceholderText("Enter command...")
        self.send_button = QtWidgets.QPushButton("Send Command")
        self.send_button.clicked.connect(self.send_command)
        
        command_layout.addWidget(self.command_input)
        command_layout.addWidget(self.send_button)
        
        #Image displaying 
        self.image_label = QtWidgets.QLabel()
        self.image_label.setFixedSize(400, 300)
        self.image_label.setStyleSheet("border: 1px solid black;")
        image_layout.addWidget(QtWidgets.QLabel("CCD Image Display"))
        image_layout.addWidget(self.image_label)
        
        #Logging stuff and more layouts
        self.log_text = QtWidgets.QTextEdit()
        self.log_text.setReadOnly(True)
        log_layout.addWidget(QtWidgets.QLabel("Command Log"))
        log_layout.addWidget(self.log_text)
        main_layout.addLayout(command_layout)
        main_layout.addLayout(image_layout)
        main_layout.addLayout(log_layout)
        
        self.setLayout(main_layout)
    
    def send_command(self):
        command = self.command_input.text()
        if command:
            self.log_text.append(f"Sent command: {command}")
            # later integrate actual communication with the PCB stuff
            self.command_input.clear()

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    gui = CCDControlGUI()
    gui.show()
    sys.exit(app.exec_())