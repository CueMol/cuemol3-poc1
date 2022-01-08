const core = require('core');

const _internal = core.getModule();
const sysconf_path = core.getSysConfigPath();
console.log('core loaded:', _internal);
console.log('sysconf path:', sysconf_path);

_internal.initCueMol(sysconf_path);
