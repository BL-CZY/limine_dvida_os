{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = with pkgs.buildPackages; [ 
	    nasm
	    qemu
	    xorriso
      gnumake
	    dhex
      cloc
      pkgsCross.x86_64-embedded.buildPackages.gcc
      pkgsCross.x86_64-embedded.buildPackages.libgcc
      pkgsCross.x86_64-embedded.buildPackages.binutils
    ];
    
    shellHook = ''
        export ENV_CC=x86_64-elf-gcc
        export ENV_LD=x86_64-elf-ld
    '';
}
