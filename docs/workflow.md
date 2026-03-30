---
hide:
  - navigation
  - toc
---
**Think of HDF5 as** more than just a file format — it’s **a data container on steroids.** On your laptop, it behaves a bit like a filesystem image: it can hold binary and text data, organized in neat hierarchies. But instead of “mounting” it like a disk, you interact with it directly through APIs — and nearly every major programming language and operating system has first-class support. Here’s the kicker: unlike a plain filesystem image, HDF5 scales. Drop it onto a parallel file system in a cluster or cloud environment, and suddenly you’re not bound by a single disk’s I/O. You can unleash terabytes per second of aggregated bandwidth, with multiple processes reading and writing in parallel without stepping on each other’s toes. For quants and traders, that means one thing: backtesting at scale. Whether you’re crunching tick-by-tick market data on a home workstation or hammering through years of trades on rented HPC clusters, HDF5 lets you move from “toy backtest” to serious industrial-grade simulation without changing your workflow. **In short: fast, portable, scalable** — exactly what you need when testing financial models against massive tick datasets.

Before diving in, make sure your setup can keep up — we’re not just crunching toy datasets here, we’re working with **real tick-level market data**. Here’s what you’ll need:

<div class="flex gap-4 w-full" markdown="1">
<div class="w-1/2 border border-solid border-gray-300 rounded-lg shadow-lg overflow-hidden flex flex-col" markdown="1">
<div class="bg-gray-400 text-white px-4 h-9 rounded-t" markdown="1">:material-receipt-text-edit-outline: Prerequisites</div> 
<div class="p-4 flex-1" markdown="1">
* **OS**: :simple-linux: Linux with a recent kernel  
* **Compilers**:  :material-language-cpp:{.icon} `g++` ≥ 13 or `clang++` ≥ 17 · :material-language-rust:{.icon} `rustc` ≥ 1.90  
* **Core Tools**:  
    * :octicons-cloud-24: [**iex-download**](https://github.com/vargalabs/iex-download)  
    * :octicons-database-24: [**iex2h5**](https://github.com/vargalabs/iex2h5)  
* **Statistical Environment** (choose one):  
    * :simple-julia:{.icon} Julia → HDF5, Statistics, GRUtils  
    * :simple-python:{.icon} Python → h5py, NumPy, Matplotlib  
    * :simple-r:{.icon} Matlab / Octave / R → should work, but no examples provided
* **Quants**:  
    * :material-cone:{.icon} Foundations in mathematical programming: Linear, **Conic formulations**  
    * :material-numeric:{.icon} **Optimization** and numerical stability  
    * :material-function-variant:{.icon} Probability & stochastic processes (**Brownian motion**, Poisson jumps)  
    * :material-sigma:{.icon} **Portfolio theory** & risk metrics (variance, VaR, CVaR)  
    * :material-clock-time-four-outline:{.icon} Time series analysis (ARIMA, GARCH, **Kalman filters**)  
</div></div>

<div class="w-1/2 flex flex-col gap-4" markdown="1">
<div class="flex-1 border border-solid border-gray-300 rounded-lg shadow-lg overflow-hidden flex flex-col" markdown="1">
<div class="bg-purple-700 text-white px-4 h-9 rounded-t" markdown="1">:material-battery-low: The Baseline Rig — Build Small, Test Big</div>
<div class="flex-1" markdown="1">
- :material-laptop: **Laptop**: any Linux-compatible machine will do 
- :material-folder-open: **Storage**: ≥60GB free space
- :material-integrated-circuit-chip: **Memory**: 16–32 GB RAM (sweet spot for local datasets)  
</div></div>

<div class="flex-1 border border-solid border-gray-300 rounded-lg shadow-lg overflow-hidden flex flex-col" markdown="1">
<div class="bg-purple-700 text-white px-4 h-9 rounded-t" markdown="1">:material-battery-50: The Workstation — From Prototype to Proof </div>
<div class="flex-1" markdown="1">
- :material-harddisk: **Storage**: 4TB NVMe SSD for scratch space · 16–32TB magnetic RAID-10 for tick data  
- :material-integrated-circuit-chip: **Memory**: 64 GB DDR5 RAM is a comfortable starting point for multi-year datasets  
- :material-chip: **CPUs**: A recent multi-core processor 
</div></div>

<div class="flex-1 border border-solid border-gray-300 rounded-lg shadow-lg overflow-hidden flex flex-col" markdown="1">
<div class="bg-purple-700 text-white px-4 h-9 rounded-t" markdown="1">:material-battery: HPC Cluster Mode — Backtest Multiple Ideas at Once</div>
<div class="flex-1" markdown="1">
- :material-view-parallel: **Parallel FS**: **OrangeFS**, BeeGFS,  Lustre, GPFS or **S3** based custom solution
- :material-hours-12: **Batch Processor**: **SLURM**, PBS Pro ...
- :simple-databricks: **MPI**: **OpenMPI**, MPICH, Intel MPI
- :material-ethernet: **Fabric**: 10–40 GbE, or **Infiniband** (HDR/EDR) for low-latency comms
</div>
</div></div></div>

### Workflow

Regardless of your computing resources, the first step is always the same: **download the original IEX PCAP files**. You can do this in small batches (per week or month) or all at once if you have the storage. A full single-shot download requires at least **8TB of reliable space** — about **6TB for the TOPS dataset** plus another **2TB for the HDF5 IRTS streams**. If you work in mini-batches, the workflow is simple: download → convert to HDF5 → remove the raw PCAPs. Once the HDF5 IRTS streams are in place, daily upkeep is lightweight — just download the new ticks and convert.

Think of the **HDF5 IRTS streams** as a **read-only tick database**. They store *all* ticks first, and you later carve out the slices you need at whatever time resolution your backtest requires. This prevents information loss and avoids wasted computation. Instead of repeatedly parsing every raw tick (many irrelevant to your strategy), you can **pre-select instruments, time resolution, and date range**, and extract price/volume matrices directly from the IRTS store. This second step — **deriving structured matrices from raw tick streams** — is what makes the approach powerful. It lets you save compute cycles *and* get results faster. While you could keep the original IEX PCAP dataset and convert on the fly, this is suboptimal for three reasons:

1. PCAP storage requires roughly **3× more space** than the HDF5 equivalent.
2. Conversion from PCAP doesn’t generalize well to other exchanges, while HDF5 does.
3. Processing tickdata from HDF5 IRTS stream is 3x faster

<!-- ==================================================== -->
<div x-data="{ tab: 'pcap' }" class="w-full overflow-hidden">
  <!-- Tab headers -->
  <div class="flex space-x-1">
    <button @click="tab = 'pcap'" :class="tab === 'pcap' ? 'bg-purple-50 dark:bg-gray-700 font-bold' : 'bg-gray-100 dark:bg-gray-800'" 
      class="px-4 py-2 text-sm rounded-t-lg focus:outline-none">
      PCAP ⇨ HDF5
    </button>
    <button @click="tab = 'hdf5'" :class="tab === 'hdf5' ? 'bg-purple-50 dark:bg-gray-700 font-bold' : 'bg-gray-100 dark:bg-gray-800'" 
      class="px-4 py-2 text-sm rounded-t-lg focus:outline-none"> HDF5 ⇨ HDF5 </button>
  </div>
  <div class="rounded-b-lg bg-purple-50 dark:bg-gray-700" markdown="1">
    <div x-show="tab === 'pcap'" class="font-mono text-sm p-1" markdown="1">
```bash
steven@saturn:~$ iex2h5 -o scratch/rts-from-pcap.h5 -c rts -g 0 /lake/iex/tops/TOPS-2025-09-0?.pcap.gz
[iex2h5] Converting 6 files using backend: hdf5 — using 1 thread — © Varga Consulting, 2017–2025
[iex2h5] Visit https://vargalabs.github.io/iex2h5/ — Star it, Share it, Support Open Tools ⭐️
▫ 2025-09-02 14:30:00 21:00:00 ✓
▫ 2025-09-03 14:30:00 21:00:00 ✓
▫ 2025-09-04 14:30:00 21:00:00 ✓
▫ 2025-09-05 14:30:00 21:00:00 ✓
▫ 2025-09-08 14:30:00 21:00:00 ✓
▫ 2025-09-09 14:30:00 21:00:00 ✓
benchmark: 1849582050 events in 364090ms  5.1 Mticks/s, 0.196000 µs/tick latency, 48.58 GiB input converted into 496.74 MiB output
[iex2h5] Conversion complete — all files processed successfully 
[iex2h5] Market data © IEX — Investors Exchange. Attribution required. See https://iextrading.com
```
</div>
<div x-show="tab === 'hdf5'" class="font-mono text-sm p-1" markdown="1">
```bash
steven@saturn:~$ iex2h5 -o scratch/rts-from-hdf5.h5 -c rts -g 0 --date-range 2025-09-02:2025-09-09 /lake/iex/iex.h5 
[iex2h5] Converting 1 file using backend: hdf5 — using 1 thread — © Varga Consulting, 2017–2025
[iex2h5] Visit https://vargalabs.github.io/iex2h5/ — Star it, Share it, Support Open Tools ⭐️
▫ 2025-09-02 14:30:00 21:00:00 ✓
▫ 2025-09-03 14:30:00 21:00:00 ✓
▫ 2025-09-04 14:30:00 21:00:00 ✓
▫ 2025-09-05 14:30:00 21:00:00 ✓
▫ 2025-09-08 14:30:00 21:00:00 ✓
▫ 2025-09-09 14:30:00 21:00:00 ✓
benchmark: 1849582050 events in 126815ms  14.6 Mticks/s, 0.068000 µs/tick latency, 1983.04 GiB input converted into 1011.73 MiB output
[iex2h5] Conversion complete — all files processed successfully 
[iex2h5] Market data © IEX — Investors Exchange. Attribution required. See https://iextrading.com
```
</div></div></div>
<!-- ==================================================== -->


`iex-download --tops --directory ./data 2016-12-01..2016-12-31` 

``` bash
# Download a batch of gzip-compressed PCAP files

# Convert the IRTS stream into an HDF5 container
iex2h5 -c irts -o iex-archive.h5 ./data/*.pcap.gz
# Rinse and repeat until you 

# Convert IRTS streams into daily price matrices sampled every 10 seconds for the month of September 2025
iex2h5 -c rts --time-interval 00:00:10 --date-range 2025-09-01:2025-09-30 -o experiment-001.h5 iex-archive.h5
```

<div id="asciicast-iex-download-demo" class="asciicast-player [&_.ap-terminal]:text-[0.55rem] w-full overflow-hidden mb-4"></div>
<script>
  document.addEventListener("DOMContentLoaded", function () {
    function waitForAsciinemaPlayer(attempts = 10) {
      if (typeof AsciinemaPlayer !== "undefined") {
        AsciinemaPlayer.create('../casts/iex-download-demo.cast', document.getElementById('asciicast-iex-download-demo'), {
          cols: 148, rows: 39, autoPlay: false,  loop: true, speed: 1.0,  idleTimeLimit: .3, controls: true
        });
      } else if (attempts > 0) {
        setTimeout(() => waitForAsciinemaPlayer(attempts - 1), 200);
      } else {
        console.error("AsciinemaPlayer failed to load.");
      }
    }
    waitForAsciinemaPlayer();
});
</script>

<!--div class="w-full border border-solid border-gray-300 rounded-lg shadow-lg overflow-hidden flex flex-col" markdown="1">
<div class="bg-orange-400 text-white px-4 h-9 rounded-t" markdown="1">:simple-julia: Julia Camp (speed, classical, clean)</div> 

| Dimension               | Julia ⚡ (Speed + Clarity)                  | Python 🐍 (Ease + Breadth)                 |
| ----------------------- | ------------------------------------------ | ------------------------------------------ |
| **Numerical speed**     | Native, C-like, no GIL, SIMD/GPU support   | Needs C-extensions (NumPy, Numba, Cython)  |
| **Mathematical syntax** | Reads like math (dispatch, linear algebra) | Verbose, but familiar with NumPy/Pandas    |
| **ML ecosystem**        | Smaller, but JuMP, Flux, SciML are solid   | Huge, industry standard (PyTorch, TF)      |
| **Production use**      | Direct to prod for quant/HPC               | Often a prototype layer; prod = C++/Java   |
| **Community**           | Research/HPC-heavy, quants, physicists     | Very broad: data science, ML, fintech, web |
| **Hiring**              | Niche skillset, high leverage              | Huge talent pool, easy onboarding          |
</div-->

<div class="flex gap-4 w-full mt-4 mb-10" markdown="1">
<div class="w-1/2 border border-solid border-gray-300 rounded-lg shadow-lg overflow-hidden flex flex-col" markdown="1">
<div class="bg-orange-400 text-white px-4 h-9 rounded-t" markdown="1">:simple-julia: Julia Camp (speed, classical, clean)</div> 
<div class="p-4 flex-1" markdown="1">
Julia shines for backtesting engines, portfolio optimization, Monte Carlo risk simulations, and factor models where speed + correctness matter. It’s the “serious scalpel” of the toolkit.

- **Performance-first mindset:** Julia gives you near-C speeds without dropping down to C/C++ for bottlenecks. Great for HPC backtests, Monte Carlo, PDE solvers, stochastic control, portfolio optimization.
- **Mathematical clarity:** code often reads like equations (broadcasting, multiple dispatch, type system). That appeals to quants with strong math/physics backgrounds.
- **Unified stack:** same language for prototyping, modeling, and production — no “prototype in Python, rewrite in C++” gap.
- **Growing but smaller ecosystem:** plenty of numerical libraries (JuMP, DifferentialEquations.jl, etc.), but less off-the-shelf ML tooling compared to Python.
- **Culture:** tends to attract researchers, HPC-minded devs, and those coming from MATLAB / Fortran / C++ who want performance and elegance.
</div></div>

<div class="w-1/2 flex flex-col gap-4" markdown="1">
<div class="flex-1 border border-solid border-gray-300 rounded-lg shadow-lg overflow-hidden flex flex-col" markdown="1">
<div class="bg-orange-400 text-white px-4 h-9 rounded-t" markdown="1">:simple-python: Python Camp (easy, broad, pragmatic)</div>
<div class="p-4 flex-1" markdown="1">
Python dominates data ingestion, preprocessing, prototyping strategies, ML-based factor discovery, and dashboards. It’s the “Swiss army knife” for everyday quant tasks.

- **Accessibility:** easy to learn, easy to hire for. Large pool of developers and data scientists.
- **Ecosystem depth:** pandas, NumPy, SciPy, scikit-learn, PyTorch, TensorFlow, statsmodels, cvxpy — you get a library for almost anything.
- **Glue language:** integrates well with C/C++/Rust for performance, databases for data, and dashboards for reporting.
- **Batteries-included ML/AI:** dominant ecosystem for deep learning, NLP, reinforcement learning.
- **Culture:** pragmatic, collaborative, “just get it done.” Many production trading systems glue Python on top of C++/Java backends.
</div></div>
</div></div>

### Conclusion

With `iex2h5`, you don’t just store raw tick data — you transform it into a scalable research platform. From **laptop prototypes** to **workstation-scale backtests** to **HPC clusters running thousands of strategy sweeps**, the workflow stays the same: download → convert → analyze. That consistency is the real win: you can start small, validate ideas quickly, and scale without rewriting pipelines. For quants, that means shorter feedback loops, more reliable simulations, and faster iteration. For engineers, it means a portable, efficient storage format that won’t choke when the datasets grow by orders of magnitude.
Try it on a week of data, scale to months, then throw years of history at it. HDF5 doesn’t care — and that’s the point.
</div></div>

