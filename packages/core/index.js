const path = require('path');
const _internal = require('bindings')('node_jsbr');

function getModule() {
  return _internal;
}

exports.getModule = getModule;

exports.getSysConfigPath = function () {
  return path.join(__dirname, '..', '..', 'core_src', 'data', 'sysconfig.xml');
}
