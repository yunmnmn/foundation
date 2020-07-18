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

   bld.stlib(
      source       = ['Source/Container/ContainerTypes.cpp',
                      'Source/FileSystem/FileSystem.cpp',
                      'Source/Memory/Allocator.cpp',
                      'Source/Util/Assert.cpp',
                      'Source/Util/Atomic.cpp',
                      'Source/Util/HashName.cpp',
                      'Source/Util/Logger.cpp',
                      'Source/Util/MurmurHash3.cpp',
                      ], 

      target       = 'Foundation', 
      use          = ["TLSF", "EASTL"],

      includes     = ['Include'],
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