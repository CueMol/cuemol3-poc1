const path = require('path');
const core = require('core');
console.log('core:', core);
const { createCueMol } = core;
console.log('createCueMol:', createCueMol);

// const _internal = core.getModule();
// const sysconf_path = core.getSysConfigPath();
// console.log('core loaded:', _internal);
// console.log('sysconf path:', sysconf_path);

// _internal.initCueMol(sysconf_path);

const load_path = path.join(__dirname, '..', '..', '..', 'src', 'data', 'sysconfig.xml');
console.log('load_path:', load_path);
const cuemol = createCueMol(load_path);

function test(cuemol) {
  let yy;
  xx = cuemol.createObj("Vector");
  console.log("xx._utils:", xx._utils);
  console.log("Vector:", xx.toString());
  xx.x = 0.123;
  xx.y = 1.23;
  xx.z = 12.3;
  console.log("xx._utils:", xx._utils);
  console.log("length:", xx.length());
  console.log("xx._utils:", xx._utils);
  yy = xx.scale(10);
  console.log(`scaled: ${yy}`);
  
  //////////
  
  xx = cuemol.createObj("Color");
  xx.setHSB(1, 1, 120);
  console.log(`color: ${xx}`);
}

test(cuemol);
