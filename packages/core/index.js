const path = require('path');
const _internal = require('bindings')('node_jsbr');
const { CueMol } = require('./build/javascript/cuemol');
console.log("_internal: ", _internal);

function getModule() {
  return _internal;
}

exports.getModule = getModule;

exports.getSysConfigPath = function () {
  console.log('XXX path.resolve:',path.resolve('.'));
  console.log('XXX __filename:', __filename);
  console.log('XXX __dirname:', __dirname);
  const load_path = path.join(__dirname, 'build', 'data', 'sysconfig.xml');
  console.log('load_path:', load_path);
  return load_path;
}

exports.createCueMol = function (sysconfig_path) {
  const cuemol = new CueMol({internal: _internal});
  if (!sysconfig_path) {
    cuemol.initCueMol(exports.getSysConfigPath());
  }
  else {
    cuemol.initCueMol(sysconfig_path);
  }
  return cuemol;
}
