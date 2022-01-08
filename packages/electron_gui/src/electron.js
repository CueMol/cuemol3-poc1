
'use strict';

const path = require('path');
const { app, Menu, BrowserWindow, dialog } = require('electron');
const isMac = (process.platform === 'darwin');
console.log(`isMac: ${isMac}`);

let mainWindow;

const template = [
  ...(isMac ? [{
      label: app.name,
      submenu: [
        {role:'about'},
        {type:'separator'},
        {role:'services'},
        {type:'separator'},
        {role:'hide'},
        {role:'hideothers'},
        {role:'unhide'},
        {type:'separator'},
        {role:'quit'}
      ]
    }] : []),
  {
    label: 'File',
    submenu: [
        {label: 'Open File...', click: ()=>{
            console.log("XXX");
            const paths = dialog.showOpenDialogSync(mainWindow, {
                buttonLabel: 'Open',
                filters: [
                    { name: 'PDB', extensions: ['pdb'] },
                ],
                properties:[
                    'openFile',
                    'createDirectory',
                ]
            });
            console.log("XXX", paths);
            mainWindow.webContents.send('open-file', paths);
        }},
        isMac ? {role:'close'} : {role:'quit'},
    ]
  }
];

app.on('window-all-closed', function() {
    if (process.platform != 'darwin') {
	    app.quit();
    }
});

const createWindow = () => {
    mainWindow = new BrowserWindow({
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false,
            // nodeIntegration: false,
            // contextIsolation: true,
            // preload: path.join(__dirname, 'preload.js'),
        },
    });
    mainWindow.loadFile('./index.html');

    mainWindow.on('ready-to-show', function () {
        mainWindow.show();
        mainWindow.focus();
    });


    mainWindow.on('closed', function() {
	    mainWindow = null;
    });

    mainWindow.webContents.openDevTools();
    
    Menu.setApplicationMenu(Menu.buildFromTemplate(template));
}

app.whenReady().then(createWindow);
