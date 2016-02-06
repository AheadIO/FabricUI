#
# Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
#

import os, subprocess
Import('buildOS', 'buildArch', 'buildType', 'parentEnv', 'fabricFlags', 'qtFlags', 'qtMOC', 'uiLibPrefix', 'qtDir', 'stageDir', 'shibokenPysideDir')

shibokenPysideIncludeDirs = [
  shibokenPysideDir.Dir('include'),
  shibokenPysideDir.Dir('include').Dir('PySide'),
  shibokenPysideDir.Dir('include').Dir('PySide').Dir('QtCore'),
  shibokenPysideDir.Dir('include').Dir('PySide').Dir('QtGui'),
  shibokenPysideDir.Dir('include').Dir('PySide').Dir('QtOpenGL'),
  shibokenPysideDir.Dir('include').Dir('shiboken'),
  ]
shibokenPysideFlags = {
  'CPPPATH': shibokenPysideIncludeDirs,
  'LIBPATH': [shibokenPysideDir.Dir('lib')],
  'LIBS': ['pyside-python2.7', 'shiboken-python2.7'],
}

suffix = '4'
if buildType == 'Debug':
  suffix = 'd4'

qtMOCBuilder = Builder(
  action = [[qtMOC, '-o', '$TARGET', '$SOURCE']],
  prefix = 'moc_',
  suffix = '.cc',
  src_suffix = '.h',
)

# create the build environment
env = parentEnv.Clone()
env.Append(BUILDERS = {'QTMOC': qtMOCBuilder})
env.Append(CPPPATH = [env.Dir('#').Dir('Native').srcnode()])

if buildOS == 'Darwin':
  env.Append(CCFLAGS = ['-fvisibility=hidden'])
  env.Append(CXXFLAGS = ['-std=c++03'])
  env.Append(CXXFLAGS = ['-stdlib=libstdc++'])
  env.Append(CXXFLAGS = ['-fvisibility=hidden'])
  env.Append(LINKFLAGS = ['-stdlib=libstdc++'])
  frameworkPath = os.path.join(
    os.path.split(
      subprocess.Popen(
        'xcodebuild -version -sdk macosx10.8 Path',
        shell=True,
        stdout=subprocess.PIPE
        ).stdout.read()
      )[0],
    'MacOSX10.7.sdk',
    )
  env.Append(CCFLAGS = ["-isysroot", frameworkPath])
  env.Append(FRAMEWORKS = ['OpenGL', 'Cocoa', 'Foundation'])
  # # [pzion 20150427] Work around horrible Apple SDK bug
  # env.Append(CPPDEFINES = {'objc_returns_inner_pointer': ''})

if buildOS == 'Linux':
  env.Append(CPPPATH=['/usr/include/qt4'])
  env.Replace( CC = '/opt/centos5/usr/bin/gcc' )
  env.Replace( CXX = '/opt/centos5/usr/bin/gcc' )

if buildOS == 'Windows':
  env.Append(CPPDEFINES = ['FABRIC_OS_WINDOWS'])
elif buildOS == 'Linux':
  env.Append(CPPDEFINES = ['FABRIC_OS_LINUX'])
elif buildOS == 'Darwin':
  env.Append(CPPDEFINES = ['FABRIC_OS_DARWIN'])

if buildOS != 'Windows':
  if '-static-libstdc++' in env['LINKFLAGS']:
    env['LINKFLAGS'].remove('-static-libstdc++')
  if '-static-libgcc' in env['LINKFLAGS']:
    env['LINKFLAGS'].remove('-static-libgcc')

# # enable timers
# env.Append(CPPDEFINES = ['FABRICUI_TIMERS'])

def GlobQObjectHeaders(env, filter):
  headers = Flatten(env.Glob(filter))
  qobjectHeaders = []
  for header in headers:
    content = open(header.srcnode().abspath, 'rb').read()
    if content.find('Q_OBJECT') > -1:
      qobjectHeaders.append(header)
  return qobjectHeaders
Export('GlobQObjectHeaders')
env.AddMethod(GlobQObjectHeaders)

def GlobQObjectSources(env, filter):
  headers = env.GlobQObjectHeaders(filter)
  sources = []
  for header in headers:
    sources += env.QTMOC(header)
  return sources
Export('GlobQObjectSources')
env.AddMethod(GlobQObjectSources)

if buildType == 'Debug':
  env.Append(CPPDEFINES = ['_DEBUG'])

env.MergeFlags(fabricFlags)
env.MergeFlags(qtFlags)

dirs = [
  'Util',
  'Style',
  'Viewports',
  'KLEditor',

  'TreeView',
  'ValueEditor_Legacy',
  'ValueEditor',
  'GraphView',
  'GraphView/Commands',
  'DFG',
  'DFG/DFGUICmd',
  'DFG/Dialogs',
 
  'SceneHub',
  'SceneHub/Managers',
  'SceneHub/TreeView',
  'SceneHub/Viewports',
  'SceneHub/Commands',

  'Licensing',
  'ModelItems'
]

includeDir = stageDir.Dir('include').Dir('FabricServices')

installedHeaders = []
sources = []
strsources = []
strheaders = []
for d in dirs:
  headers = Flatten(env.Glob(os.path.join(env.Dir('.').abspath, d, '*.h')))
  dirsrc = Flatten(env.Glob(os.path.join(env.Dir('.').abspath, d, '*.cpp')))
  for h in headers:
    strheaders.append(str(h))
  for c in dirsrc:
    strsources.append(str(c))
  sources += dirsrc
  sources += env.GlobQObjectSources(os.path.join(env.Dir('.').abspath, d, '*.h'))
  if uiLibPrefix == 'ui':
    installedHeaders += env.Install(stageDir.Dir('include').Dir('FabricUI').Dir(d), headers)

uiLib = env.StaticLibrary('FabricUI', sources)

uiFiles = installedHeaders
if uiLibPrefix == 'ui':
  uiLib = env.Install(stageDir.Dir('lib'), uiLib)
  uiFiles.append(uiLib)
  icons = env.Install(
    stageDir.srcnode().Dir('Resources').Dir('Icons'),
    [
      Glob(os.path.join(env.Dir('GraphView').Dir('images').srcnode().abspath, '*.png')),
      # Glob(os.path.join(env.Dir('ValueEditor').Dir('images').srcnode().abspath, '*.png')),
      ]
    )
  env.Depends(uiLib, icons)
  qss = env.Install(
    stageDir.srcnode().Dir('Resources').Dir('QSS'),
    [
      Glob(os.path.join(env.Dir('ValueEditor').srcnode().abspath, '*.qss')),
      ]
    )
  env.Depends(uiLib, qss)

# if buildOS == 'Windows':
#   projName = 'FabricUI.vcxproj'# + env['MSVSPROJECTSUFFIX']
#   projNode = Dir('#').File(projName)
#   if not projNode.exists():
#     print("---- Building " + projName + " VS Proj for FabricUI ----")
#     projFile = env.MSVSProject(target = projName,
#                   srcs = strsources,
#                   incs = strheaders,
#                   buildtarget = uiLib,
#                   auto_build_solution=0,
#                   variant = 'Debug|x64')
#     env.Depends(uiLib, projName)
      
locals()[uiLibPrefix + 'Lib'] = uiLib
locals()[uiLibPrefix + 'IncludeDir'] = env.Dir('#').Dir('Native').srcnode()
locals()[uiLibPrefix + 'Flags'] = {
  'CPPPATH': [locals()[uiLibPrefix + 'IncludeDir']],
  'LIBS': [locals()[uiLibPrefix + 'Lib']]
}
Export(uiLibPrefix + 'Lib', uiLibPrefix + 'IncludeDir', uiLibPrefix + 'Flags')

env.Alias(uiLibPrefix + 'Lib', uiFiles)

pysideEnv = env.Clone()
pysideDir = pysideEnv.Dir('pyside')
shibokenDir = pysideEnv.Dir('shiboken')

if buildOS != 'Windows':
  pysideEnv.Append(CCFLAGS = ['-Wno-sign-compare', '-Wno-error'])

if uiLibPrefix == 'ui' and buildOS == 'Linux':
  pysideGen = pysideEnv.Command(
    pysideDir.Dir('FabricUI').File('fabricui_python.h'),
    shibokenDir.File('global.h'),
    [
        [
            os.path.join(shibokenPysideDir.abspath, 'bin', 'shiboken'),
            '$SOURCE',
            #'--no-suppress-warnings',
            '--typesystem-paths='+shibokenPysideDir.Dir('share').Dir('PySide').Dir('typesystems').abspath+
                ':'+pysideEnv.Dir('shiboken').srcnode().abspath,
            '--include-paths='+shibokenPysideDir.Dir('include').Dir('PySide').abspath+
                ':'+stageDir.Dir('include').abspath,
            '--enable-pyside-extensions',
            '--output-directory='+pysideDir.abspath,
            shibokenDir.File('fabricui.xml').srcnode().abspath
        ],
        [ 'cd', pysideDir.abspath, '&&', 'patch', '-p1',
            '<'+shibokenDir.File('fabricui.diff').srcnode().abspath ],
    ]
    )
  pysideEnv.Alias('pysideGen', [pysideGen])

  fabricDir = env.Dir(os.environ['FABRIC_DIR'])
  pythonCAPISourceDir = fabricDir.Dir('Source').Dir('Python').Dir('Core').Dir('2.7')
  pysideEnv.Append(CPPPATH = [
      pysideEnv.Dir('DFG').srcnode(),
      pysideEnv.Dir('GraphView').srcnode(),
      pysideEnv.Dir('Licensing').srcnode(),
      pysideEnv.Dir('Style').srcnode(),
      pysideEnv.Dir('ValueEditor').srcnode(),
      pysideEnv.Dir('ValueEditor_Legacy').srcnode(),
      pysideEnv.Dir('Viewports').srcnode(),
      fabricDir.Dir('include'),
      fabricDir.Dir('include').Dir('FabricServices').Dir('ASTWrapper'),
      pythonCAPISourceDir,
      pysideDir,
      '/usr/local/include/python2.7',
      '/usr/include/Qt',
      '/usr/include/QtCore',
      '/usr/include/QtGui',
      ])
  pysideEnv.MergeFlags(shibokenPysideFlags)
  
  pysideEnv.Append(LIBS = [
    'FabricUI',
    'FabricServices',
    'FabricSplitSearch',
    'FabricCore',
    'QtCore',
    'QtGui',
    'QtOpenGL',
  ])
  pysideEnv.Append(CPPDEFINES = [
    'FEC_PROVIDE_STL_BINDINGS',
  ])
  
  copyPythonCAPI = []
  copyPythonCAPI += pysideEnv.Command(
    pysideDir.File('CAPI_wrap.cpp'),
    pythonCAPISourceDir.File('CAPI_wrap.cpp'),
    [
      Copy('$TARGET', '$SOURCE'),
      ['sed', '-i', 's/# define SWIGINTERN static SWIGUNUSED/# define SWIGINTERN SWIGUNUSED/', '$TARGET'],
      ['sed', '-i', 's/static swig_type_info \\*swig_types/swig_type_info \\*swig_types/', '$TARGET'],
      ['sed', '-i', 's/#include <Core\/Clients\/CAPI\//#include </', '$TARGET'],
      ['sed', '-i', 's/#include <Core\/Clients\/PythonCAPI\//#include </', '$TARGET'],
    ]
  )
  
  copyPythonCAPI += pysideEnv.Command(
    pysideDir.File('PythonCAPI.cpp'),
    pythonCAPISourceDir.File('PythonCAPI.cpp'),
    [
      Copy('$TARGET', '$SOURCE'),
    ]
  )
  
  copyCAPIHeader = pysideEnv.Command(
    pysideDir.File('SWIG_CAPI.h'),
    shibokenDir.File('SWIG_CAPI.template.h'),
    [
      ['echo', '#ifndef SWIG_CAPI_h', '>$TARGET'],
      ['echo', '#define SWIG_CAPI_h', '>>$TARGET'],
      ['cat', '$SOURCE', '>>$TARGET'],
      ['echo', '-n', 'extern ', '>>$TARGET'],
      ['grep', 'swig_type_info \\*swig_types', pysideDir.File('CAPI_wrap.cpp'), '>>$TARGET'],
      ['grep', '#define SWIGTYPE_p_', pysideDir.File('CAPI_wrap.cpp'), '>>$TARGET'],
      ['echo', '#endif // SWIG_CAPI_h', '>>$TARGET'],
    ]
  )
  pysideEnv.Depends(copyCAPIHeader, copyPythonCAPI)
  
  if buildOS == 'Linux':
    pysideEnv.Append(LINKFLAGS = [Literal('-Wl,-rpath,$ORIGIN/../../../lib/')])
  pysideLib = pysideEnv.LoadableModule(
    'FabricUI',
    [
      pysideEnv.Glob('pyside/*.cpp'),
      pysideEnv.Glob('pyside/FabricUI/*.cpp'),
    ],
    LDMODULEPREFIX='',
    )
  pysideEnv.Depends(pysideLib, [copyPythonCAPI, copyCAPIHeader])

  installedPySideLib = pysideEnv.Install(
    stageDir.Dir('Python').Dir('2.7').Dir('FabricEngine'),
    pysideLib)
  pysideEnv.Alias('pysideLib', [pysideLib, installedPySideLib])

Return('uiFiles')
