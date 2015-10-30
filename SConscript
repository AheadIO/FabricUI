#
# Copyright 2010-2015 Fabric Software Inc. All rights reserved.
#

import os
Import('buildDir', 'buildOS', 'buildArch', 'buildType', 'parentEnv', 'fabricFlags', 'qtFlags', 'qtMOC', 'uiLibPrefix', 'qtDir', 'stageDir', 'shibokenPysideDir', 'shibokenPysideFlags')

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
  'TreeView',
  'ValueEditor',
  'GraphView',
  'GraphView/Commands',
  'KLEditor',
  'DFG',
  'DFG/DFGUICmd',
  'DFG/Dialogs',
  'SceneHub',
  'Viewports',
  'Licensing',
]

includeDir = stageDir.Dir('include').Dir('FabricServices')

installedHeaders = []
sources = []
for d in dirs:
  headers = Flatten(env.Glob(os.path.join(env.Dir('.').abspath, d, '*.h')))
  sources += Flatten(env.Glob(os.path.join(env.Dir('.').abspath, d, '*.cpp')))
  sources += env.GlobQObjectSources(os.path.join(env.Dir('.').abspath, d, '*.h'))
  if uiLibPrefix == 'ui':
    installedHeaders += env.Install(stageDir.Dir('include').Dir('FabricUI').Dir(d), headers)

uiLib = env.StaticLibrary('FabricUI', sources)
uiFiles = installedHeaders
if uiLibPrefix == 'ui':
  uiFiles += env.Install(stageDir.Dir('lib'), uiLib)
  icons = env.Install(stageDir.srcnode().Dir('Resources').Dir('Icons'), Glob(os.path.join(env.Dir('GraphView').Dir('images').srcnode().abspath, '*.png')))
  env.Depends(uiLib, icons)

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

if buildOS != 'Windows':
  pysideEnv.Append(CCFLAGS = ['-Wno-sign-compare', '-Wno-error'])

pysideGen = pysideEnv.Command(
    pysideDir.File('fabricui_python.h'),
    pysideEnv.File('PySideGlobal.h'),
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
            pysideEnv.Dir('shiboken').File('fabricui.xml').srcnode().abspath
        ],
        [ 'cd', pysideDir.abspath, '&&', 'patch', '-p1',
            '<'+pysideEnv.Dir('shiboken').File('fabricui.diff').srcnode().abspath ],
    ]
    )
if uiLibPrefix == 'ui':
  env.Alias('pysideGen', [pysideGen])

pythonCAPIDir = env.Dir('#').Dir('Core').Dir('Clients').Dir('PythonCAPI')
pysideEnv.Append(CPPPATH = [
    pysideEnv.Dir('DFG').srcnode(),
    pysideEnv.Dir('GraphView').srcnode(),
    pysideEnv.Dir('Licensing').srcnode(),
    pysideEnv.Dir('Style').srcnode(),
    pysideEnv.Dir('ValueEditor').srcnode(),
    pysideEnv.Dir('Viewports').srcnode(),
    stageDir.Dir('include'),
    stageDir.Dir('include').Dir('FabricServices').Dir('ASTWrapper'),
    pythonCAPIDir,
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
  buildDir.Dir('Core').Dir('Clients').Dir('PythonCAPI').Dir('2.7').File('CAPI_wrap.cpp'),
  [
    Copy('$TARGET', '$SOURCE'),
    ['sed', '-i', 's/# define SWIGINTERN static SWIGUNUSED/# define SWIGINTERN SWIGUNUSED/', '$TARGET'],
    ['sed', '-i', 's/static swig_type_info \\*swig_types/swig_type_info \\*swig_types/', '$TARGET'],
  ]
)

copyPythonCAPI += pysideEnv.Command(
  pysideDir.File('PythonCAPI.cpp'),
  pythonCAPIDir.File('PythonCAPI.cpp'),
  [
    Copy('$TARGET', '$SOURCE'),
  ]
)

copyCAPIHeader = pysideEnv.Command(
  pysideDir.File('SWIG_CAPI.h'),
  pysideEnv.Dir('shiboken').File('SWIG_CAPI.template.h'),
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

pysideLib = pysideEnv.LoadableModule(
    'FabricUI',
    [
      Glob('pyside/FabricUI/*.cpp'),
      Glob('pyside/*.cpp'),
    ],
    LDMODULEPREFIX='',
    )
pysideEnv.Depends(pysideLib, [copyPythonCAPI, copyCAPIHeader])

if uiLibPrefix == 'ui':
  env.Alias('pysideLib', [pysideLib])

Return('uiFiles')
