project('cnc-tools', 'c',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'buildtype=plain',
    'debug=true',
    'b_lto=true',
    'c_std=gnu11',
])

cc = meson.get_compiler('c')

cc_is_gcc_like = cc.get_argument_syntax() == 'gcc' 
cc_is_msvc_like = cc.get_argument_syntax() == 'msvc' 

if not cc_is_gcc_like and not cc_is_msvc_like
  message('Please use something gcc-like or msvc-like, or update the build system')
endif

threads_dep = dependency('threads')

dpdk = subproject('dpdk', required: true, default_options: {
    'pkt_mbuf_headroom': 0,
    'enable_stdatomic': false,
    'cpu_instruction_set': 'native',
    'disable_apps': 'dpdk-testpmd,dumpcap,graph,pdump,proc-info,test-acl,test-bbdev,test-cmdline,test-compress-perf,test-crypto-perf,test-dma-perf,test-eventdev,test-fib,test-flow-perf,test-gpudev,test-mldev,test-pipeline,test-regex,test-sad,test-security-perf',
    'disable_drivers': 'common/**,net/**,crypto/**,compress/**,dma/**,event/**,gpu/**,ml/**,regex/**,vdpa/**,raw/**,fpga/**,baseband/**,bus/**',
    'enable_drivers': 'power/**,mempool/**',
    'disable_libs': 'acl,argparse,bbdev,bitratestats,bpf,cfgfile,cmdline,compressdev,cryptodev,dispatcher,distributor,dmadev,efd,ethdev,eventdev,fib,gpudev,graph,gro,gso,hash,ip_frag,ipsec,jobstats,kvargs,latencystats,log,lpm,member,meter,metrics,mldev,net,node,pcapng,pdcp,pdump,pipline,port,ptr_compress,rawdev,rcu,regexdev,reorder,rib,sched,rte_security',
})

dpdk_dep = dpdk.get_variable('dpdk_dep')

subdir('Framework')
subdir('CPU Tests')