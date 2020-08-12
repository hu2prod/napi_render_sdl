# napi_sdl
TODO some docs\
just usage example

    #!/usr/bin/env node
    mod = require("napi_sdl");
    
    size_x  = 1920
    size_y  = 1080
    use_bgr = 0 // 33 ms -> 22 ms (1.5 times faster)
    
    buffer = Buffer.alloc(3*size_x*size_y)
    for(var i=0;i<buffer.length;i+=3) {
      buffer[i+0] = 255;
      buffer[i+1] = 0;
      buffer[i+2] = 0;
    }
    
    mod.start_rgb(size_x, size_y, use_bgr)
    mod.draw_rgb(buffer)
    
    setTimeout(()=>{
      console.log("done");
    }, 10000);
