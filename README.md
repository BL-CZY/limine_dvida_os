<h1>Dvida OS</h1>

Hi, This is a simple operating system that I am developing based on the osdev wiki

<h1>Build</h1>

<h2>On Nix-OS</h2>
You can run <code>nix develop</code> at the root directory of the project, which will then install all other packages required

<h2>On Non Nix-OS</h2>
<h3>First of all</h3>
A gcc & binutils cross compiler toolchain with the target i686-elf is needed<br />
After building the compilers, set the following environment variables:<br />
<code>$(ENV_CC)="path/to/cross-gcc"</code><br />
<code>$(ENV_LD)="path/to/cross-ld"</code><br />

<h3>Secondly</h3>
These are other packages required:<br />
nasm<br />
qemu<br />
xorriso<br />
gnumake<br />
you also need to get the limine repo<br />
<code>git clone https://github.com/limine-bootloader/limine.git --branch=v7.x-binary --depth=1</code><br />
<code>make -C limine</code><br />
<br />

<h1>Run</h1>
To run, a raw disk image called storage.img must be created<br />
You can use qemu-img to do so<br />
Then, simply run <code>./run.sh</code> would emulate the iso in qemu
