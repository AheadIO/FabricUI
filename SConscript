#
# Copyright 2010-2015 Fabric Software Inc. All rights reserved.
#

import os
Import(
  'buildOS',
  'buildArch',
  'buildType',
  'buildDir',
  'parentEnv',
  'fabricFlags',
  'qtFlags',
  'qtMOC',
  'uiLibPrefix',
  'qtDir',
  'stageDir',
  'shibokenBin',
  'shibokenRoot',
  'shibokenFlags',
  'pythonConfigs',
  'pysideRoot',
  'pysideFlags',
  )

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

if uiLibPrefix == 'ui' and buildOS != 'Windows':

  fabricDir = env.Dir(os.environ['FABRIC_DIR'])

  pysideGens = []
  installedPySideLibs = []

  for pythonVersion, pythonConfig in pythonConfigs.iteritems():

    pysideEnv = env.Clone()
    pysideEnv.MergeFlags(shibokenFlags)
    pysideEnv.MergeFlags(pysideFlags)

    pysideDir = pysideEnv.Dir('pyside')
    shibokenDir = pysideEnv.Dir('shiboken')

    if buildOS == 'Linux':
      qtIncludePaths = [
        '/usr/include/Qt',
        '/usr/include/QtCore',
        '/usr/include/QtGui',
        '/usr/include/QtOpenGL',
        ]
    if buildOS == 'Darwin':
      qtIncludePaths = [
        '/usr/local/include/Qt',
        '/usr/local/include/QtCore',
        '/usr/local/include/QtGui',
        '/usr/local/include/QtOpenGL',
        ]
    pysideEnv.Append(CPPPATH = qtIncludePaths)

    if buildOS != 'Windows':
      pysideEnv.Append(CCFLAGS = ['-Wno-sign-compare', '-Wno-error'])

    shibokenIncludePaths = [
      str(pysideRoot.Dir("include").Dir("PySide")),
      ]
    if buildOS == 'Darwin':
      # Location for Qt frameworks
      shibokenIncludePaths.append("/usr/local/lib")
    shibokenIncludePaths.append(os.path.join(os.environ['FABRIC_DIR'], "include"))

    pysideGen = pysideEnv.Command(
      pysideDir.Dir('FabricUI').File('fabricui_python.h'),
      [
        shibokenDir.File('global.h'),
        shibokenDir.File('fabricui.diff'),
        shibokenDir.File('fabricui.xml'),
        shibokenDir.File('fabricui_core.xml'),
        shibokenDir.File('fabricui_dfg.xml'),
        shibokenDir.File('fabricui_viewports.xml'),
        ],
      [
          [
              shibokenBin,
              '${SOURCES[0]}',
              #'--no-suppress-warnings',
              '--typesystem-paths='+':'.join([
                str(shibokenDir.srcnode()),
                str(pysideRoot.Dir('share').Dir('PySide').Dir('typesystems')),
                ]),
              '--include-paths='+':'.join(shibokenIncludePaths),
              '--output-directory='+pysideDir.abspath,
              #################################################################
              # These are copied from PySide's CMakeLists.txt:
              '--generator-set=shiboken',
              '--enable-parent-ctor-heuristic',
              '--enable-pyside-extensions',
              '--enable-return-value-heuristic',
              '--use-isnull-as-nb_nonzero',
              #################################################################
              '${SOURCES[2]}',
          ],
          [ 'cd', pysideDir.Dir("FabricUI").abspath, '&&', 'patch', '-p1',
              '<${SOURCES[1].abspath}' ],
      ]
      )
    pysideEnv.Depends(pysideGen, installedHeaders)
    pysideGens.append(pysideGen)

    pysideEnv.Append(CPPPATH = [
        pysideEnv.Dir('DFG').srcnode(),
        pysideEnv.Dir('DFG/DFGUICmd').srcnode(),
        pysideEnv.Dir('GraphView').srcnode(),
        pysideEnv.Dir('Licensing').srcnode(),
        pysideEnv.Dir('Style').srcnode(),
        pysideEnv.Dir('ValueEditor').srcnode(),
        pysideEnv.Dir('Viewports').srcnode(),
        fabricDir.Dir('include'),
        fabricDir.Dir('include').Dir('FabricServices').Dir('ASTWrapper'),
        ])
    if buildOS == 'Linux':
      pysideEnv.Append(CPPPATH = [
        pythonConfig['includeDir'],
        ] + qtIncludePaths)
      pysideEnv.Append(LIBS = [
        'QtCore',
        'QtGui',
        'QtOpenGL',
        ])
    if buildOS == 'Darwin':
      pysideEnv.Append(FRAMEWORKPATH = ['/usr/local/lib'])
      pysideEnv.Append(FRAMEWORKS = ['QtCore', 'QtGui', 'QtOpenGL'])
      pysideEnv.Append(CPPPATH = [pythonConfig['includeDir']])
      pysideEnv.Append(LIBPATH = [pythonConfig['libDir']])
      pysideEnv.Append(LIBS = [pythonConfig['lib']])
    
    pysideEnv.Append(LIBS = [
      'FabricUI',
      'FabricServices',
      'FabricSplitSearch',
      'FabricCore',
      ])
    pysideEnv.Append(CPPDEFINES = [
      'FEC_PROVIDE_STL_BINDINGS',
      ])

    fabricPythonLibName = "FabricPython"

    majMinVer = '.'.join([pysideEnv['FABRIC_VERSION_MAJ'], pysideEnv['FABRIC_VERSION_MIN']])
    majMinRevVer = '.'.join([pysideEnv['FABRIC_VERSION_MAJ'], pysideEnv['FABRIC_VERSION_MIN'], pysideEnv['FABRIC_VERSION_REV']])

    if buildOS == 'Windows':
      fabricPythonLibBasename = '.'.join([fabricPythonLibName, 'dll'])
    if buildOS == 'Linux':
      fabricPythonLibBasename = '.'.join(['lib' + fabricPythonLibName, 'so', majMinRevVer])
    if buildOS == 'Darwin':
      fabricPythonLibBasename = '.'.join(['lib' + fabricPythonLibName, majMinRevVer, 'dylib'])

    if buildOS == 'Linux':
      pysideEnv.Append(LINKFLAGS = Literal(','.join([
        '-Wl',
        '-rpath',
        '$ORIGIN/../../../lib/',
        '-rpath',
        '$ORIGIN/../../../Python/' + pythonVersion + '/FabricEngine/Core/',
        ])))
    if buildOS == 'Darwin':
      pysideEnv.Append(LINKFLAGS = ['-Wl,-rpath,@loader_path/../../..'])
      pysideEnv.Append(CCFLAGS = ['-Wno-mismatched-tags'])
    pysideEnv.Append(CPPPATH = pysideDir.abspath)
    pysideEnv.Append(CPPPATH = stageDir.Dir('Python/' + pythonVersion + '/FabricEngine/Core'))
    pysideEnv.Append(LIBS = stageDir.Dir('Python/' + pythonVersion + '/FabricEngine/Core').File(fabricPythonLibBasename))
    installedPySideLib = pysideEnv.LoadableModule(
      pysideEnv['STAGE_DIR'].Dir('Python').Dir(pythonVersion).Dir('FabricEngine').File('FabricUI.so'),
      [
        pysideEnv.Glob('pyside/*.cpp'),
        pysideEnv.Glob('pyside/FabricUI/*.cpp'),
      ],
      LDMODULEPREFIX='',
      )
    pysideEnv.Depends(installedPySideLib, pysideGen)
    installedPySideLibs.append(installedPySideLib)

  pysideEnv.Alias('pysideGen', pysideGens)
  pysideEnv.Alias('pysideLib', installedPySideLibs)

Return('uiFiles')
