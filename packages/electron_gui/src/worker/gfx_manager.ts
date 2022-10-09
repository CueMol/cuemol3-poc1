const FLOAT_SIZE = 4
const MODEL_MAT_SIZE = 4 * 4 * FLOAT_SIZE;
const PROJ_MAT_SIZE = 4 * 4 * FLOAT_SIZE;
const NORM_MAT_SIZE = 4 * 4 * FLOAT_SIZE;

const LIGHT_UBO_SIZE = 4 * FLOAT_SIZE + 4 * FLOAT_SIZE;

export class GfxManager {
  // for program object
  private _prog_data: any = {};

  // common UBO info
  private _mvp_mat_loc: number = 0;
  private _mat_ubo: any = null;

  private _light_loc: number = 1;
  private _light_ubo: any = null;
  
  // for VBOs
  private _draw_data: any = {};

  private cuemol: any;
  private _sceMgr: any;
  private _canvas: any = null;
  private _afcbid: any = null;
  private bound_views: any = [];

  private _context: any;

  private _enable_lighting_loc: number = 0;

  constructor(cuemol: any) {
    this.cuemol = cuemol;
    this._sceMgr = this.cuemol.getService('SceneManager');
  }
  
  bindCanvas(canvas: any, view_id: number, dpr: number|null=null) : void {
    if (this._canvas !== null) {
      throw Error('already bound to canvas');
    }
    this._canvas = canvas;
    this._context = canvas.getContext('webgl2');
    const gl = this._context;
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.disable(gl.CULL_FACE);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
    gl.enable(gl.BLEND);

    const view = this._sceMgr.getView(view_id);
    
    if (dpr!==null) {
      console.log('bindCanvas dpr=', dpr);
      view.setSclFac(dpr, dpr);
    }

    this.cuemol.internal.bindPeer(view._wrapped, this);
    this.bound_views.push(view_id);

    this.createUBO();
    this.setUpLight();
  }

  get canvas() : any {
    return this._canvas;
  }

  addView(view_id: number, dpr: number) : void {
    if (this._canvas === null) {
      throw Error('not bound to canvas');
    }
    const view = this._sceMgr.getView(view_id);
    if (dpr!==null) {
      console.log('addView dpr=', dpr);
      view.setSclFac(dpr, dpr);
    }
    this.cuemol.internal.bindPeer(view._wrapped, this);
    this.bound_views.push(view_id);
  }
  
  removeView(view_id: number) : void {
    // TODO: impl
    this.bound_views = this.bound_views.filter(x => x!==view_id);
  }

  setUpdateView(view_id: number) : void {
    if (!this.bound_views.includes(view_id))
      throw Error();
    this._afcbid && cancelAnimationFrame(this._afcbid);
    let view = this._sceMgr.getView(view_id);
    const render = () => {
      view.checkAndUpdate();
      this._afcbid = requestAnimationFrame(render);
    };
    render();
  }

  //////////
  // UBO

  // Create UBO
  createUBO() : void {
    const gl = this._context;

    // MVP matrix UBO
    let matrix_ubo = gl.createBuffer();
    gl.bindBuffer(gl.UNIFORM_BUFFER, matrix_ubo);
    gl.bufferData(gl.UNIFORM_BUFFER,
                  MODEL_MAT_SIZE*2 + PROJ_MAT_SIZE,
                  gl.DYNAMIC_DRAW);
    gl.bindBufferBase(gl.UNIFORM_BUFFER, this._mvp_mat_loc, matrix_ubo);
    gl.bindBuffer(gl.UNIFORM_BUFFER, null);
    this._mat_ubo = matrix_ubo;

    // Lighting UBO
    let light_ubo = gl.createBuffer();
    gl.bindBuffer(gl.UNIFORM_BUFFER, light_ubo);
    gl.bufferData(gl.UNIFORM_BUFFER,
                  LIGHT_UBO_SIZE,
                  gl.DYNAMIC_DRAW);
    gl.bindBufferBase(gl.UNIFORM_BUFFER, this._light_loc, light_ubo);
    gl.bindBuffer(gl.UNIFORM_BUFFER, null);
    this._light_ubo = light_ubo;

    console.log('UBO created');
  }

  //////////
  // Program objects

  toShaderTypeID(name: string) : any {
    const gl = this._context;
    if (name === 'vertex') {
      return gl.VERTEX_SHADER;
    } else if (name === 'fragment') {
      return gl.FRAGMENT_SHADER;
    } else {
      throw `unknown shader type: ${name}`;
    }
  }

  /// API
  createShader(name: string, data: {[key: string]: string}) : boolean {
    const gl = this._context;
    if (name in this._prog_data) {
      console.log(`CreateShader name ${name} already exists --> reuse`);
      // return false;
      return true;
    }
    const program = gl.createProgram();

    for (const [key, value] of Object.entries(data)) {
      // console.log(key, value);
      let shader_type = this.toShaderTypeID(key);
      const shader = gl.createShader(shader_type);
      gl.shaderSource(shader, value);
      gl.compileShader(shader);

      const status = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
      if (!status) {
        const info = gl.getShaderInfoLog(shader);
        console.log("XXX: shader compile failed");
        console.log(info);
        return false;
      }

      gl.attachShader(program, shader);
    }

    gl.linkProgram(program);

    const status = gl.getProgramParameter(program, gl.LINK_STATUS);
    if (!status) {
      const info = gl.getProgramInfoLog(program);
      console.log("XXX: shader link failed");
      console.log(info);
      return false;
    }

    // setup common UBO entries
    const mvp_mat_index = gl.getUniformBlockIndex(program, 'mvp_matrix');
    gl.uniformBlockBinding(program, mvp_mat_index, this._mvp_mat_loc);

    const light_index = gl.getUniformBlockIndex(program, 'lighting');
    gl.uniformBlockBinding(program, light_index, this._light_loc);

    this._prog_data[name] = program;

    this._enable_lighting_loc = gl.getUniformLocation(this._prog_data[name],
                                                      "enable_lighting");

    return true;
  }

  /// API
  deleteShader(shader_name: string) : boolean {
    const gl = this._context;
    if (!(shader_name in this._prog_data)) {
      console.log(`name ${shader_name} not defined`);
      return false;
    }
    gl.deleteProgram(this._prog_data[shader_name]);
    return true;
  }

  /// API
  enableShader(shader_name: string) : void {
    const gl = this._context;
    gl.useProgram(this._prog_data[shader_name]);
    this._enable_lighting_loc = gl.getUniformLocation(this._prog_data[shader_name],
                                                      "enable_lighting");
  }

  /// API
  disableShader() : void {
    const gl = this._context;
    gl.useProgram(null);
  }

  /// API
  clear(r: number, g: number, b: number) : void {
    const gl = this._context;
    gl.clearColor(r, g, b, 1);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
  }

  //////////
  // Projection uniforms

  /// API
  setUpModelMat(array_buf: any) : void {
    // transfer UBO
    const gl = this._context;
    gl.bindBuffer(gl.UNIFORM_BUFFER, this._mat_ubo);
    gl.bufferSubData(gl.UNIFORM_BUFFER, 0, array_buf);
    gl.bindBuffer(gl.UNIFORM_BUFFER, null);
  }

  /// API
  setUpProjMat(cx: number, cy: number, array_buf: any) : void {
    // transfer UBO
    const gl = this._context;
    gl.viewport(0, 0, cx, cy);
    gl.bindBuffer(gl.UNIFORM_BUFFER, this._mat_ubo);
    gl.bufferSubData(gl.UNIFORM_BUFFER, MODEL_MAT_SIZE + 12*FLOAT_SIZE, array_buf);
    gl.bindBuffer(gl.UNIFORM_BUFFER, null);
  }

  // lighting uniforms
  setUpLight() : void {
    const gl = this._context;
    let buf = new Float32Array([0.2, 0.8, 0.4, 32.0,
                                1.0, 1.0, 1.5, 0.0]);
    gl.bindBuffer(gl.UNIFORM_BUFFER, this._light_ubo);
    gl.bufferSubData(gl.UNIFORM_BUFFER, LIGHT_UBO_SIZE, buf);
    gl.bindBuffer(gl.UNIFORM_BUFFER, null);
  }

  //////////
  // Buffer

  /// API
  createBuffer(name: string, nsize: number, num_elems: number,
               nsize_index: number, elem_info_str: string) : boolean {
    if (name in this._draw_data) {
      console.log(`name ${name} already exists`);
      return false;
    }

    const gl = this._context;
    let elem_info = JSON.parse(elem_info_str);

    // VAO
    let vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

    let vertexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);

    const stride = nsize / num_elems;
    elem_info.forEach((value) => {
      let aloc = value['nloc'];
      gl.enableVertexAttribArray(aloc);
      gl.vertexAttribPointer(
        aloc,
        value['nelems'],
        gl.FLOAT,
        false,
        stride,
        value['npos']
      );
    });
    gl.bufferData(gl.ARRAY_BUFFER, nsize, gl.STATIC_DRAW);
    console.log('vbo nsize=', nsize);

    // index buffer
    let indexBuffer = null;
    if (nsize_index > 0) {
      indexBuffer = gl.createBuffer();
      gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
      gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, nsize_index, gl.STATIC_DRAW);
      // gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
      console.log('ibo nsize=', nsize_index);
    }

    // gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    this._draw_data[name] = [vao, vertexBuffer, indexBuffer];

    // const new_id = this._new_draw_id
    // this._draw_data[new_id] = [vao, vertexBuffer];
    // this._new_draw_id ++;

    console.log('create buffer OK, new_id=', name);
    return true;
  }

  /// API
  drawBuffer(id: number, nmode: number, nelems: number,
             array_buf: any, index_buf: any, isUpdated: boolean,
             enable_lighting: boolean) : void {
    const gl = this._context;
    const obj = this._draw_data[id];
    if (isUpdated) {
      // Transfer VBO to GPU
      const vbo = obj[1];
      const ibo = obj[2];
      gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
      gl.bufferSubData(gl.ARRAY_BUFFER, 0, array_buf);
      gl.bindBuffer(gl.ARRAY_BUFFER, null);
      if (index_buf !== null && ibo !== null) {
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, ibo);
        gl.bufferSubData(gl.ELEMENT_ARRAY_BUFFER, 0, index_buf);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
      }
    }

    let nglmode = gl.TRIANGLES;
    if (nmode == 4) {
      nglmode = gl.LINES;
      gl.lineWidth(5);
    }
    // else if (nmode == 7) {
    //     nglmode
    // }
    gl.uniform1i(this._enable_lighting_loc, enable_lighting);
    gl.bindVertexArray(obj[0]);
    if (index_buf === null) {
      gl.drawArrays(nglmode, 0, nelems);
    } else {
      // console.log("drawelem nelems=", nelems);
      gl.drawElements(nglmode, nelems, gl.UNSIGNED_INT, 0);
    }
    gl.bindVertexArray(null);
  }

  /// API
  deleteBuffer(id: number) : boolean {
    const gl = this._context;

    if (!(id in this._draw_data)) return false;
    const obj = this._draw_data[id];
    if (obj === null) return false;

    delete this._draw_data[id];
    // delete VBO
    gl.deleteBuffer(obj[1]);
    // delete index VBO
    if (obj[2] !== null) {
      gl.deleteBuffer(obj[2]);
    }
    // delete VAO
    gl.deleteVertexArray(obj[0]);

    return true;
  }
};
