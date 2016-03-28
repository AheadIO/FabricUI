#
# Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
#

import os
Import(
  'buildOS',
  'buildArch',
  'buildType',
  'buildDir',
  'parentEnv',
  'fabricFlags',
  'glewFlags',
  'qtFlags',
  'qtMOC',
  'uiLibPrefix',
  'qtIncludeDir',
  'qtBinDir',
  'qtLibDir',
  'stageDir',
  'pythonConfigs',
  'capiSharedLibFlags',
  'servicesFlags_mt',
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
env.MergeFlags(glewFlags)

dirs = [
  'Util',
  'Style',
  'TreeView',
  'ValueEditor_Legacy',
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
  'ModelItems'
]

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
      Glob(os.path.join(env.Dir('DFG').Dir('Icons').srcnode().abspath, '*.png')),
      # Glob(os.path.join(env.Dir('ValueEditor').Dir('images').srcnode().abspath, '*.png')),
      ]
    )
  env.Depends(uiLib, icons)
  qss = env.Install(
    stageDir.srcnode().Dir('Resources').Dir('QSS'),
    [
      Glob(os.path.join(env.Dir('ValueEditor').srcnode().abspath, '*.qss')),
      Glob(os.path.join(env.Dir('DFG').srcnode().abspath, '*.qss')),
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

if uiLibPrefix == 'ui':

  fabricDir = env.Dir(os.environ['FABRIC_DIR'])

  pysideGens = []
  installedPySideLibs = []

  for pythonVersion, pythonConfig in pythonConfigs.iteritems():

    if not pythonConfig['havePySide']:
      continue

    pysideEnv = env.Clone()
    pysideEnv.MergeFlags(capiSharedLibFlags)
    pysideEnv.MergeFlags(servicesFlags_mt)
    pysideEnv.Append(LIBS = ['FabricSplitSearch'])
    pysideEnv.MergeFlags(pythonConfig['pythonFlags'])
    pysideEnv.MergeFlags(pythonConfig['shibokenFlags'])
    pysideEnv.MergeFlags(pythonConfig['pysideFlags'])
    pysideEnv.MergeFlags(qtFlags)
    # These are necessary because of the way that shiboken
    # generates its includes
    pysideEnv.Append(CPPPATH = [
      stageDir.Dir('include').Dir('FabricServices').Dir('ASTWrapper'),
      os.path.join(qtIncludeDir, 'QtCore'),
      os.path.join(qtIncludeDir, 'QtGui'),
      os.path.join(qtIncludeDir, 'QtOpenGL'),
      ])

    pysideDir = pysideEnv.Dir('pyside').Dir('python'+pythonVersion)
    shibokenDir = pysideEnv.Dir('shiboken')

    if buildOS == 'Windows':
      pysideEnv['CCFLAGS'].remove('/W2')
      pysideEnv.Append(LINKFLAGS = ['/NODEFAULTLIB:LIBCMT'])
    else:
      pysideEnv.Append(CCFLAGS = ['-Wno-sign-compare', '-Wno-error'])

    shibokenIncludePaths = [
      str(pythonConfig['pysideIncludeDir']),
      ]
    shibokenIncludePaths.append(qtIncludeDir)
    shibokenIncludePaths.append(os.path.join(os.environ['FABRIC_DIR'], "include"))

    if buildOS == 'Linux':
      pysideEnv['ENV']['LD_LIBRARY_PATH'] = qtLibDir
    elif buildOS == 'Windows':
      pysideEnv['ENV']['PATH'] = qtBinDir+';'+pysideEnv['ENV']['PATH']

    diffFile = shibokenDir.File('fabricui.diff')

    pysideGen = pysideEnv.Command(
      pysideDir.Dir('FabricUI').File('fabricui_python.h'),
      [
        shibokenDir.File('global.h'),
        diffFile,
        shibokenDir.File('fabricui.xml'),
        shibokenDir.File('fabricui_core.xml'),
        shibokenDir.File('fabricui_dfg.xml'),
        shibokenDir.File('fabricui_viewports.xml'),
        ],
      [
          [
              pythonConfig['shibokenBin'],
              '${SOURCES[0]}',
              #'--no-suppress-warnings',
              '--typesystem-paths='+os.pathsep.join([
                str(shibokenDir.srcnode()),
                str(pythonConfig['pysideTypesystemsDir']),
                ]),
              '--include-paths='+os.pathsep.join(shibokenIncludePaths),
              '--output-directory='+pysideDir.abspath,
              '--avoid-protected-hack',
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
        pysideEnv.Dir('ValueEditor_Legacy').srcnode(),
        pysideEnv.Dir('Viewports').srcnode(),
        fabricDir.Dir('include'),
        ])
    pysideEnv.Append(CPPPATH = [pythonConfig['includeDir']])
    if buildOS == 'Darwin':
      pysideEnv.Append(LIBPATH = [pythonConfig['libDir']])
      pysideEnv.Append(LIBS = [pythonConfig['lib']])
    
    pysideEnv.Append(LIBS = [
      'FabricUI',
      ])
    pysideEnv.Append(CPPDEFINES = [
      'FEC_PROVIDE_STL_BINDINGS',
      ])

    fabricPythonLibName = "FabricPython"

    majMinVer = '.'.join([pysideEnv['FABRIC_VERSION_MAJ'], pysideEnv['FABRIC_VERSION_MIN']])
    majMinRevVer = '.'.join([pysideEnv['FABRIC_VERSION_MAJ'], pysideEnv['FABRIC_VERSION_MIN'], pysideEnv['FABRIC_VERSION_REV']])

    if buildOS == 'Linux':
      pysideEnv.Append(LINKFLAGS = Literal(','.join([
        '-Wl',
        '-rpath',
        '$ORIGIN/../../../lib/',
        '-rpath',
        '$ORIGIN/../../../Python/' + pythonVersion + '/FabricEngine/',
        ])))
    if buildOS == 'Darwin':
      pysideEnv.Append(LINKFLAGS = ['-Wl,-rpath,@loader_path/../../..'])
      pysideEnv.Append(CCFLAGS = ['-Wno-mismatched-tags'])
    pysideEnv.Append(CPPPATH = pysideDir.abspath)
    pysideEnv.Append(CPPPATH = stageDir.Dir('Python/' + pythonVersion + '/FabricEngine'))
    pysideEnv.Append(LIBPATH = [stageDir.Dir('Python/' + pythonVersion + '/FabricEngine')])
    pysideEnv.Append(LIBS = [fabricPythonLibName])
    if buildOS == 'Windows':
      if buildType == 'Debug':
        pysideEnv.Append(LIBS = "MSVCRTD")
      else:
        pysideEnv.Append(LIBS = "MSVCRT")
    installedPySideLibName = 'FabricUI'+pythonConfig['moduleSuffix']
    pythonDstDir = pysideEnv['STAGE_DIR'].Dir('Python').Dir(pythonVersion).Dir('FabricEngine')
    installedPySideLib = pysideEnv.LoadableModule(
      pythonDstDir.File(installedPySideLibName),
      [
        pysideEnv.Glob('pyside/python'+pythonVersion+'/*.cpp'),
        pysideEnv.Glob('pyside/python'+pythonVersion+'/FabricUI/*.cpp'),
      ],
      LDMODULEPREFIX='',
      LDMODULESUFFIX=pythonConfig['moduleSuffix'],
      )
    pysideEnv.Depends(installedPySideLib, pysideGen)
    installedPySideLibs.append(installedPySideLib)
    if buildOS == 'Windows':
      pysideDLLDst = pysideEnv['STAGE_DIR'].Dir('bin')
    else:
      pysideDLLDst = pysideEnv['STAGE_DIR'].Dir('lib')

    pysideDLLs = ['shiboken-python'+pythonVersion, 'pyside-python'+pythonVersion]
    pysideVersion = pythonConfig['pysideVersion']
    pysideMaj, pysideMin, pysideBug = pysideVersion.split('.')

    if buildOS == 'Windows':
      for pysideDLL in pysideDLLs:
        installedPySideLibs.append(
          pysideEnv.Install(
            pysideDLLDst,
            pythonConfig['pysideDir'].Dir('bin').File(pysideDLL+'.dll').abspath
            )
          )
    elif buildOS == 'Linux':
      libSuffix = 'so'
      for pysideDLL in pysideDLLs:
        installedPySideLibs.append(
          pysideEnv.AddPostAction(
            pysideEnv.Install(
              pysideDLLDst,
              pythonConfig['pysideDir'].Dir('lib').File('lib'+pysideDLL+'.'+libSuffix+'.'+pysideVersion)
              ),
            [
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, libSuffix, pysideMaj, pysideMin, pysideBug]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, libSuffix, pysideMaj, pysideMin])),
                ],
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, libSuffix, pysideMaj, pysideMin]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, libSuffix, pysideMaj])),
                ],
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, libSuffix, pysideMaj]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, libSuffix])),
                ]
              ]
            )
          )
    elif buildOS == 'Darwin':
      libSuffix = 'dylib'
      for pysideDLL in pysideDLLs:
        installedPySideLibs.append(
          pysideEnv.AddPostAction(
            pysideEnv.Install(
              pysideDLLDst,
              pythonConfig['pysideDir'].Dir('lib').File('lib'+pysideDLL+'.'+pysideVersion+'.'+libSuffix)
              ),
            [
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, pysideMaj, pysideMin, pysideBug, libSuffix]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, pysideMaj, pysideMin, libSuffix])),
                ],
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, pysideMaj, pysideMin, libSuffix]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, pysideMaj, libSuffix])),
                ],
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, pysideMaj, libSuffix]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, libSuffix])),
                ]
              ]
            )
          )

    pysideModuleDstDir = pysideEnv['STAGE_DIR'].Dir('Python').Dir(pythonVersion).Dir('PySide')
    if buildOS == 'Windows':
      pysideModuleSrcDir = pythonConfig['pysideDir'].Dir('lib').Dir('site-packages').Dir('PySide')
    else:
      pysideModuleSrcDir = pythonConfig['pysideDir'].Dir('lib').Dir('python'+pythonVersion).Dir('site-packages').Dir('PySide')
    installedPySideLibs.append(
      pysideEnv.Install(
        pysideModuleDstDir,
        Glob(os.path.join(pysideModuleSrcDir.abspath, '*'))
        )
      )

  pysideEnv.Alias('pysideGen', pysideGens)
  pysideEnv.Alias('pysideLib', installedPySideLibs)

Return('uiFiles')
