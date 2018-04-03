#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include <X11/X.h>
#include <X11/Xutil.h>

#include <luajit-2.0/luajit.h>

#include <luajit-2.0/lua.h>
#include <luajit-2.0/lauxlib.h>
#include <luajit-2.0/lualib.h>

struct config_t {
  const char *title;
  const char *font;
};

// lua: tbl.field
static int lua_get_table_field_check(lua_State *L, char *tbl,
    char *field, int (*check)(lua_State*, int)) {
  lua_getglobal(L, tbl);
  lua_getfield(L, -1, field);
  return check(L, -1);
}

int main(int argc, char **argv) {
  Display* dpy = XOpenDisplay(NULL);
  if(!dpy) {
    fprintf(stderr, "Unable to open display\n");
    return 1;
  }
  int screen = DefaultScreen(dpy);
  struct config_t config;
  memset(&config, 0, sizeof(config));


  lua_State *L = luaL_newstate();

  lua_newtable(L);
  lua_setglobal(L, "config");

  if (luaL_dofile(L, "desktopmon.lua")) {
    fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
    goto end_program;
  }

  if(lua_get_table_field_check(L, "config", "title", lua_isstring)) {
    config.title = lua_tostring(L, -1);
  } else {
    fputs("[Error]: `config.title` must be a string", stderr);
    goto end_program;
  }

  if(lua_get_table_field_check(L, "config", "font", lua_isstring)) {
    config.font = lua_tostring(L, -1);
  } else {
    fputs("[Error]: `config.font` must be a string", stderr);
    goto end_program;
  }
 
  printf("config.title: %s\n", config.title);
  printf("config.font: %s\n", config.font);

  XFontStruct *xfont = XLoadQueryFont(dpy, config.font);
  if (!xfont) {
    fprintf(stderr, "[error] Unable to load font `%s`\n", config.font);
    goto end_program;
  }
unload_font:
  XFreeFont(dpy, xfont);
end_program:
  lua_close(L);
  XCloseDisplay(dpy);
  return 0;
}
/*
  unsigned long black = BlackPixel(dpy, screen);
  unsigned long white = WhitePixel(dpy, screen);

  Window root = DefaultRootWindow(dpy);
  Window actual_root;
  unsigned int x, y, w, h, bw, depth;
  XGetGeometry(dpy, root, &actual_root, &x, &y, &w, &h, &bw, &depth);

  GC gc = XCreateGC(dpy, root, 0, 0);
  XSetBackground(dpy, gc, black);
  
  
  struct timespec tm;
  tm.tv_sec = 0;
  tm.tv_nsec = 1000 * 1000 * 1000 / 120;
  struct timespec rtm;
  XSetForeground(dpy, gc, white);
  int fontnamessize;
  char **fontnames = XListFonts(dpy, NULL, 50, &fontnamessize);

  printf("%d\n", fontnamessize);
  for(int i = 0; i < fontnamessize; ++i) {
    printf("%s\n", fontnames[i]);
  }
  XFreeFontNames(fontnames);
  char *msg[] = {
    "coco",
    "lapin" };
  for(int i = 0;; i = (i + 1) % 2) {
    XSetForeground(dpy, gc, black);
    XFillRectangle(dpy, root, gc, x, y, w/4, h);
    XSetForeground(dpy, gc, white);
    XDrawString(dpy, root, gc, 40, 40, msg[i], strlen(msg[i]));
    XSync(dpy, False);
    nanosleep(&tm, &rtm);
  }
  */
