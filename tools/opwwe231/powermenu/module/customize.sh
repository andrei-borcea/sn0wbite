# Magisk module installer hook: the scripts and the watcher binary must be executable.
set_permissions() {
  set_perm_recursive $MODPATH 0 0 0755 0644
  set_perm $MODPATH/bin/powerkey 0 0 0755
  set_perm $MODPATH/service.sh 0 0 0755
  set_perm $MODPATH/uninstall.sh 0 0 0755
}
