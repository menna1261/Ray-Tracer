let
  unstable = import (fetchTarball https://nixos.org/channels/nixos-unstable/nixexprs.tar.xz) { };
in
{ pkgs ? (import <nixpkgs> {}) }:
with pkgs;
  mkShell {
  packages = [ #These are only specific to NixOS because it doesn't provide it at runtime

  ];

  buildInputs = with pkgs; [ # Required packages for build
    sfml
    cmake
    clang

    # OpenGL
    libGL

    # X11
    xorg.libX11
    xorg.libXrandr
    xorg.libXcursor
    #xorg.libxcb

    # Just dependecies required by sfml
    libudev-zero
    libvorbis
    flac
    openal

    # Font
    freetype
  ];
}

