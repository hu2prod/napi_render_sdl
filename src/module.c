#include <node_api.h>

#include <signal.h>

#include "SDL/SDL.h"
#include "type.h"


i32 set_size_x = 0;
i32 set_size_y = 0;
SDL_Surface *topLeft = NULL;
SDL_Surface *_display_screen = NULL;

napi_value start_rgb(napi_env env, napi_callback_info info) {
  napi_status status;
  
  napi_value ret_dummy;
  status = napi_create_int32(env, 0, &ret_dummy);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_dummy");
    return ret_dummy;
  }
  
  size_t argc = 3;
  napi_value argv[3];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to parse arguments");
    return ret_dummy;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  i32 size_x;
  status = napi_get_value_int32(env, argv[0], &size_x);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid i32 was passed as argument of size_x");
    return ret_dummy;
  }
  
  i32 size_y;
  status = napi_get_value_int32(env, argv[1], &size_y);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid i32 was passed as argument of size_y");
    return ret_dummy;
  }
  
  i32 use_bgr;
  status = napi_get_value_int32(env, argv[2], &use_bgr);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid i32 was passed as argument of use_bgr");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  if (SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE) == -1) {
    napi_throw_error(env, NULL, "fail SDL_Init");
    return ret_dummy;
  }
  signal(SIGINT, SIG_DFL);
  signal(SIGTERM, SIG_DFL);
  
  _display_screen = SDL_SetVideoMode(size_x, size_y, 24, SDL_HWSURFACE | SDL_FULLSCREEN | SDL_ASYNCBLIT);
  if (_display_screen == NULL) {
    napi_throw_error(env, NULL, "fail SDL_SetVideoMode");
    return ret_dummy;
  }
  
  SDL_ShowCursor(0);
  
  if (use_bgr) {
    topLeft = SDL_CreateRGBSurface(0, size_x, size_y, 24, 0,0,0,0);
  } else {
    topLeft = SDL_CreateRGBSurface(0, size_x, size_y, 24,
      0x0000ff, // rmask
      0x00ff00, // gmask
      0xff0000, // bmask
      0  // amask
    );
  }
  
  set_size_x = size_x;
  set_size_y = size_y;
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  return ret_dummy;
}

napi_value draw_rgb(napi_env env, napi_callback_info info) {
  napi_status status;
  
  napi_value ret_dummy;
  status = napi_create_int32(env, 0, &ret_dummy);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_dummy");
    return ret_dummy;
  }
  
  size_t argc = 1;
  napi_value argv[1];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to parse arguments");
    return ret_dummy;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  u8 *data_src;
  size_t data_src_len;
  status = napi_get_buffer_info(env, argv[0], (void**)&data_src, &data_src_len);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid buffer was passed as argument of data_src");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  if (!topLeft) {
    napi_throw_error(env, NULL, "!topLeft; start_rgb is probably not called");
    return ret_dummy;
  }
  if (data_src_len != (size_t)(3*set_size_x*set_size_y)) {
    napi_throw_error(env, NULL, "data_src_len != 3*set_size_x*set_size_y");
    return ret_dummy;
  }
  
  u8 *pixels = (u8*)topLeft->pixels;
  memcpy(pixels, data_src, data_src_len);
  
  SDL_Rect offset;
  offset.x = 0;
  offset.y = 0;
  SDL_BlitSurface(topLeft, NULL, _display_screen, &offset);
  
  if (SDL_Flip(_display_screen) == -1) {
    napi_throw_error(env, NULL, "fail SDL_Flip");
    return ret_dummy;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  
  return ret_dummy;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value fn;
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  status = napi_create_function(env, NULL, 0, start_rgb, NULL, &fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to wrap native function");
  }
  
  status = napi_set_named_property(env, exports, "start_rgb", fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to populate exports");
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  status = napi_create_function(env, NULL, 0, draw_rgb, NULL, &fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to wrap native function");
  }
  
  status = napi_set_named_property(env, exports, "draw_rgb", fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to populate exports");
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)