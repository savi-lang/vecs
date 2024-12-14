const Vecs = (function () {
  class VecsPanicError extends Error {}

  function harness(memory) {
    function copyFrom(ptr, len) {
      const dest = new Uint8Array(new ArrayBuffer(len));
      dest.set(new Uint8Array(memory.buffer, ptr, len));
      return dest;
    }

    function toString(ptr, len) {
      return new TextDecoder().decode(copyFrom(ptr, len));
    }

    return {
      memory,

      vecs_harness_page_alloc(addPageCount) {
        const ptr = memory.buffer.byteLength;
        memory.grow(addPageCount);
        return ptr;
      },

      vecs_harness_panic(ptr, len) {
        throw new VecsPanicError(toString(ptr, len));
      },
    };
  }

  function run({ url, workerCount }) {
    const memory = new WebAssembly.Memory({
      initial: 2, // TODO: how to avoid coupling with the linker flags?
      maximum: 1024, // TODO: how to avoid coupling with the linker flags?
      shared: true,
    });

    WebAssembly.instantiateStreaming(fetch(url), { env: harness(memory) }).then(
      ({ module, instance }) => {
        workerCount = workerCount ?? 0;
        for (let i = 0; i < workerCount; i++) {
          const worker = new Worker("./vecs.js"); // TODO: refer to the url of this script
          worker.postMessage({ module, memory });
        }

        instance.exports.start(0);
      }
    );
  }

  function runInsideWorker(worker) {
    worker.onmessage = (event) => {
      const { module, memory } = event.data;
      WebAssembly.instantiate(module, { env: harness(memory) }).then(
        (instance) => {
          instance.exports.start(0);
        }
      );
    };
  }

  // If this script is running in a worker, run the "inside worker" function.
  if (self.WorkerGlobalScope) runInsideWorker(self);

  return { run };
})();
