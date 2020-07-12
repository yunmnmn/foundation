def options(opt):
   pass

def configure(cnf):
   pass

def build(bld):
   bld.stlib(
      source       = ['Source/FiberScheduler.cpp', 'Source/Thread.cpp'], 
      target       = 'FiberScheduler', 
      use          = [''],

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