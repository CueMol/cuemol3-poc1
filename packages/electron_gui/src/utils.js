// export const getSceneByViewID = (cuemol, view_id) => {
//   if (view_id === null) return null;
//   const sceMgr = cuemol.getService('SceneManager');
//   const view = sceMgr.getView(view_id);
//   const scene = view.getScene();
//   return scene;
// };

// export const openPDBFile = (cuemol, scene, file_path) => {
//   let cmdMgr = cuemol.getService('CmdMgr');
  
//   let load_object = cmdMgr.getCmd('load_object');
//   load_object.target_scene = scene;
//   load_object.file_path = file_path;
//   load_object.run();
//   let mol = load_object.result_object;
  
//   let new_rend = cmdMgr.getCmd('new_renderer');
//   new_rend.target_object = mol;
//   new_rend.renderer_type = 'simple';
//   new_rend.renderer_name = 'simple1';
//   new_rend.recenter_view = true;
//   new_rend.default_style_name = 'DefaultCPKColoring';
//   new_rend.run();
// };

// export const updateView = (cuemol, view_id) => {
//   const sceMgr = cuemol.getService('SceneManager');
//   const view = sceMgr.getView(view_id);
//   view.invalidate();
//   view.checkAndUpdate();
// };
