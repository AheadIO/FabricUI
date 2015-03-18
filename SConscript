#
# Copyright 2010-2015 Fabric Software Inc. All rights reserved.
#

import os
Import('buildOS', 'buildArch', 'buildType', 'parentEnv', 'fabricFlags', 'qtFlags', 'qtMOC', 'uiLibPrefix', 'qtDir', 'stageDir')

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
env.Append(CPPPATH = [env.Dir('.').srcnode()])

if buildOS == 'Linux':
  env.Append(CPPPATH=['/usr/include/qt4'])

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
  'TreeView',
  'ValueEditor',
  'GraphView',
  'GraphView/Commands',
  'KLEditor',
  'DFG',
  'DFG/Commands',
  'DFG/Dialogs',
  'SceneGraph',
  'Viewports',
]

includeDir = stageDir.Dir('include').Dir('FabricServices')

installedHeaders = []
sources = []
for d in dirs:
  headers = Flatten(Glob(os.path.join(d, '*.h')))
  sources += Flatten(Glob(os.path.join(d, '*.cpp')))
  sources += env.GlobQObjectSources(os.path.join(d, '*.h'))
  if uiLibPrefix == 'ui':
    installedHeaders += env.Install(stageDir.Dir('include').Dir('FabricUI').Dir(d), headers)

uiLib = env.StaticLibrary('FabricUI', sources)
uiFiles = installedHeaders
if uiLibPrefix == 'ui':
  uiFiles += env.Install(stageDir.Dir('lib'), uiLib)

locals()[uiLibPrefix + 'Lib'] = uiLib
locals()[uiLibPrefix + 'IncludeDir'] = env.Dir('.').srcnode()
locals()[uiLibPrefix + 'Flags'] = {
  'CPPPATH': [locals()[uiLibPrefix + 'IncludeDir']],
  'LIBS': [locals()[uiLibPrefix + 'Lib']]
}
Export(uiLibPrefix + 'Lib', uiLibPrefix + 'IncludeDir', uiLibPrefix + 'Flags')

env.Alias(uiLibPrefix + 'Lib', uiFiles)

Return('uiFiles')
