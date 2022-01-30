'use strict';

const path = require('path');
const { app, Menu, BrowserWindow, dialog, ipcMain } = require('electron');
const isMac = process.platform === 'darwin';
console.log(`isMac: ${isMac}`);
console.log('app.getAppPath():', app.getAppPath());
console.log('app.getPath(exe):', app.getPath('exe'));

let mainWindow;

const template = [
  ...(isMac
    ? [
        {
          label: app.name,
          submenu: [
            { role: 'about' },
            { type: 'separator' },
            { role: 'services' },
            { type: 'separator' },
            { role: 'hide' },
            { role: 'hideothers' },
            { role: 'unhide' },
            { type: 'separator' },
            { role: 'quit' },
          ],
        },
      ]
    : []),
  {
    label: 'File',
    submenu: [
      {
        label: 'Open File...',
        click: () => {
          console.log('Open File clicked...');
          const paths = dialog.showOpenDialogSync(mainWindow, {
            buttonLabel: 'Open',
            filters: [{ name: 'PDB', extensions: ['pdb'] }],
            properties: ['openFile', 'createDirectory'],
          });
          console.log('Open File path:', paths);
          mainWindow.webContents.send('open-file', paths);
        },
      },
      isMac ? { role: 'close' } : { role: 'quit' },
    ],
  },
];

app.on('window-all-closed', function () {
  if (process.platform != 'darwin') {
    app.quit();
  }
});

const createWindow = () => {
  mainWindow = new BrowserWindow({
    webPreferences: {
      // nodeIntegration: true,
      nodeIntegration: false,
      nodeIntegrationInWorker: true,
      // contextIsolation: false,
      contextIsolation: true,
      preload: path.join(__dirname, 'preload.js'),
    },
  });
  mainWindow.loadFile(path.join(__dirname, '../dist/index.html'));

  mainWindow.on('ready-to-show', function () {
    mainWindow.show();
    mainWindow.focus();
  });

  mainWindow.on('closed', function () {
    mainWindow = null;
  });

  mainWindow.webContents.openDevTools({mode: 'undocked'});

  Menu.setApplicationMenu(Menu.buildFromTemplate(template));
};

app.whenReady().then(createWindow);

// ipcMain.on('apppath', (event, args) => {
//   event.returnValue = {
//     'appPath': app.getAppPath(),
//     'exePath': app.getPath('exe'),
//     'modulePath': app.getPath('module'),
//     'isPackaged': app.isPackaged,
//   };
// });

ipcMain.handle(
  'apppath',
  async () => {
    const result = {
      'appPath': app.getAppPath(),
      'exePath': app.getPath('exe'),
      'modulePath': app.getPath('module'),
      'isPackaged': app.isPackaged,
    };
    return result;
  }
);
