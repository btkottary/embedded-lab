# Git Bash version of env.ps1: puts the tools that ship inside STM32CubeIDE
# (GCC, CMake, Ninja, Make, GDB, ST-LINK GDB server) and STM32CubeProgrammer
# on PATH for THIS terminal only. The IDE itself is never started.
#
# Usage - "source" it, so the PATH change stays in your shell:
#   source tools/env.sh

CUBEIDE="${CUBEIDE:-/c/ST/STM32CubeIDE_2.0.0/STM32CubeIDE}"
_plugins="$CUBEIDE/plugins"

if [ ! -d "$_plugins" ]; then
  echo "STM32CubeIDE plugins folder not found: $_plugins (set CUBEIDE=... first)" >&2
  return 1 2>/dev/null || exit 1
fi

# Plugin folder names carry version numbers: match by prefix, take the newest.
_plugin_bin() {
  # trailing "/" = folders only (there are also .jar files with similar names)
  local d
  d=$(ls -d "$_plugins"/com.st.stm32cube.ide.mcu.externaltools."$1".*/ 2>/dev/null | sort | tail -n 1)
  [ -n "$d" ] && echo "${d%/}/tools/bin"
}
for _p in gnu-tools-for-stm32 cmake ninja stlink-gdb-server; do
  _d=$(_plugin_bin "$_p") || { echo "warning: no $_p plugin in $_plugins" >&2; continue; }
  case ":$PATH:" in *":$_d:"*) ;; *) PATH="$_d:$PATH" ;; esac
done
# The make plugin also ships busybox bash/sh/rm/mkdir/echo. Put it LAST so
# Git Bash's own bash, rm, ... keep winning (we build with Ninja anyway).
_d=$(_plugin_bin make) && case ":$PATH:" in *":$_d:"*) ;; *) PATH="$PATH:$_d" ;; esac
unset -f _plugin_bin
_cp="/c/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin"
case ":$PATH:" in *":$_cp:"*) ;; *) PATH="$_cp:$PATH" ;; esac
export PATH

echo "Tools on PATH for this terminal:"
for _t in arm-none-eabi-gcc cmake ninja make arm-none-eabi-gdb ST-LINK_gdbserver STM32_Programmer_CLI STM32_SigningTool_CLI; do
  if command -v "$_t" >/dev/null 2>&1; then
    printf '  %-22s %s\n' "$_t" "$(command -v "$_t")"
  else
    printf '  %-22s MISSING\n' "$_t"
  fi
done
unset _plugins _p _d _cp _t
