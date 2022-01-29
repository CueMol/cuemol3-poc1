
export class GfxManager {
  constructor( { cuemol }) {
    // for program object
    this._prog_data = {};

    // common UBO info
    this._mvp_mat_loc = 0;
    this._mat_ubo = null;

    // for VBOs
    this._draw_data = {};
    this.cuemol = cuemol;
  }
  
  bindCanvas(canvas) {
    this._canvas = canvas;
    this._context = canvas.getContext('webgl2');
    let gl = this._context;
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.disable(gl.CULL_FACE);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
    gl.enable(gl.BLEND);

    // if (window.devicePixelRatio) {
    //   this._view.setSclFac(window.devicePixelRatio, window.devicePixelRatio);
    // }

    this.cuemol.internal.bindPeer(this._view._wrapped, this);
  }

  //////////
  // Program objects

  toShaderTypeID(name) {
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
  createShader(name, data) {
    const gl = this._context;
    if (name in this._prog_data) {
      // console.log(`name ${name} already exists`);
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
        console.log(info);
        return false;
      }

      gl.attachShader(program, shader);
    }

    gl.linkProgram(program);

    const status = gl.getProgramParameter(program, gl.LINK_STATUS);
    if (!status) {
      const info = gl.getProgramInfoLog(program);
      console.log(info);
      return false;
    }

    // setup common UBO entries
    let mvp_mat_index = gl.getUniformBlockIndex(program, 'mvp_matrix');
    gl.uniformBlockBinding(program, mvp_mat_index, this._mvp_mat_loc);

    this._prog_data[name] = program;
    return true;
  }

  /// API
  deleteShader(shader_name) {
    const gl = this._context;
    if (!shader_name in this._prog_data) {
      console.log(`name ${shader_name} not defined`);
      return false;
    }
    gl.deleteProgram(this._prog_data[shader_name]);
    return true;
  }

  /// API
  enableShader(shader_name) {
    const gl = this._context;
    gl.useProgram(this._prog_data[shader_name]);
  }

  /// API
  disableShader() {
    const gl = this._context;
    gl.useProgram(null);
  }

  /// API
  clear(r, g, b) {
    const gl = this._context;
    gl.clearColor(r, g, b, 1);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
  }

  //////////
  // Projection uniforms

  checkMvpMatUBO() {
    if (this._mat_ubo === null) {
      // Create UBO
      const gl = this._context;
      let matrix_ubo = gl.createBuffer();
      gl.bindBuffer(gl.UNIFORM_BUFFER, matrix_ubo);
      gl.bufferData(gl.UNIFORM_BUFFER, 4 * 4 * 4 * 2, gl.DYNAMIC_DRAW);
      gl.bindBufferBase(gl.UNIFORM_BUFFER, this._mvp_mat_loc, matrix_ubo);
      gl.bindBuffer(gl.UNIFORM_BUFFER, null);
      this._mat_ubo = matrix_ubo;
      console.log('mvp UBO created');
    }
  }

  /// API
  setUpModelMat(array_buf) {
    this.checkMvpMatUBO();
    // transfer UBO
    const gl = this._context;
    gl.bindBuffer(gl.UNIFORM_BUFFER, this._mat_ubo);
    gl.bufferSubData(gl.UNIFORM_BUFFER, 0, array_buf);
    gl.bindBuffer(gl.UNIFORM_BUFFER, null);
  }

  /// API
  setUpProjMat(cx, cy, array_buf) {
    this.checkMvpMatUBO();
    // transfer UBO
    const gl = this._context;
    gl.viewport(0, 0, cx, cy);
    gl.bindBuffer(gl.UNIFORM_BUFFER, this._mat_ubo);
    gl.bufferSubData(gl.UNIFORM_BUFFER, 4 * 4 * 4, array_buf);
    gl.bindBuffer(gl.UNIFORM_BUFFER, null);
  }

  //////////
  // Buffer

  /// API
  createBuffer(name, nsize, num_elems, nsize_index, elem_info_str) {
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
  drawBuffer(id, nmode, nelems, array_buf, index_buf, isUpdated) {
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
  deleteBuffer(id) {
    const gl = this._context;

    if (!id in this._draw_data) return false;
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
