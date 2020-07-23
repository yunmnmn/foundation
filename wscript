SUBFOLDERS = [
   'ThirdParty'
]

def options(opt):
   opt.recurse(SUBFOLDERS, mandatory=False)
   pass

def configure(cnf):
   cnf.recurse(SUBFOLDERS, mandatory=False)
   pass

def build(bld):
   bld.recurse(SUBFOLDERS, mandatory=False)

   # Get an array of environment specific source
   envSpecificSourcePath = bld.GetSourcePathFromEnvironment("Source", ["Util/Mutex.cpp", "Parallel/SpinAndSleepLock.cpp"])
   # Get an array of environment specific includes
   envSpecificInclude = bld.GetIncludePathFromEnvironment("Include")

   bld.stlib(
      source       = ['Source/Container/ContainerTypes.cpp',
                      'Source/FileSystem/FileSystem.cpp',
                      'Source/Util/Assert.cpp',
                      'Source/Util/Atomic.cpp',
                      'Source/Util/HashName.cpp',
                      'Source/Util/Logger.cpp',
                      'Source/Util/MurmurHash3.cpp',
                      'Source/Memory/BaseAllocator.cpp',
                      'Source/Memory/BaseSchema.cpp',
                      'Source/Memory/EaStlAllocator.cpp',
                      'Source/Memory/MemoryManager.cpp',
                      'Source/Memory/PoolSchema.cpp',
                      'Source/Memory/TlsfSchema.cpp',
                     ] 
                     + envSpecificSourcePath, 

      target       = "Foundation", 
      use          = ["TLSF", "EASTL"],

      includes     = ["Include"] + envSpecificInclude,
      export_includes = ["Include"] + envSpecificInclude,
      defines      = [],

      lib          = [],
      libpath      = [],

      rpath        = [],
      vnum         = '1.2.3',

      idx          = 123,
      #install_path = '${SOME_PATH}/bin',
      cxxflags     = [],
      linkflags    = []
   )