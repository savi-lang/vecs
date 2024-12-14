set -e

cd "$(dirname "$0")"

# Compile the runtime bitcode for wasm32
find ../runtime -name "*.wasm32.bc" -exec rm {} \;
find ../runtime -name "*.c" -exec clang -target wasm32 -matomics -DVECS_NDEBUG -O3 -flto -emit-llvm -c {} -o {}.wasm32.bc \;
find ../runtime -name "*.wasm32.bc"

# Compile the demo for wasm32
find . -name "*.wasm32.bc" -exec rm {} \;
clang -target wasm32 -matomics -DVECS_NDEBUG -O3 -flto -emit-llvm -c main.c -o ./main.wasm32.bc
ls ./main.wasm32.bc
llvm-link-18 ../runtime/*.wasm32.bc ./main.wasm32.bc -o demo.wasm32.bc
ls ./demo.wasm32.bc

# Link the runtime and demo for wasm32, to create the output wat file
llc-18 -march=wasm32 -filetype=obj demo.wasm32.bc -o ./demo.o
ls ./demo.o
wasm-ld-18 ./demo.o -o ./demo.wasm --allow-undefined --no-entry --lto-O3 --export=start
ls ./demo.wasm

# For some reason, wasm2wat does not work with some of the opcodes we use?
# wasm2wat ./demo.wasm -o ./demo.wat
# ls ./demo.wat
