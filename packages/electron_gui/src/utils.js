export const getSceneByViewID = (cuemol, view_id) => {
  if (view_id === null) return null;
  const sceMgr = cuemol.getService('SceneManager');
  const view = sceMgr.getView(view_id);
  const scene = view.getScene();
  return scene;
};
