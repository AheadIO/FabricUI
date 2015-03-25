#
# Copyright 2010-2015 Fabric Software Inc. All rights reserved.
#

import os
import platform

if not os.environ.has_key('QT_DIR'):
  raise Exception("No QT_DIR environment variable specified.")
if not os.environ.has_key('FABRIC_DIR'):
  raise Exception("No FABRIC_DIR environment variable specified.")

buildOS = 'Darwin'
if platform.system().lower().startswith('win'):
  buildOS = 'Windows'
elif platform.system().lower().startswith('lin'):
  buildOS = 'Linux'

buildArch = 'x86_64'
buildType = 'Release'

env = Environment(MSVC_VERSION = "10.0")
env.Append(CPPPATH = [env.Dir('#').srcnode().abspath])

qtDir = os.environ['QT_DIR']

qtFlags = {}
qtMOC = None
if buildOS == 'Windows':
  if buildType == 'Debug':
    suffix = 'd4'
  else:
    suffix = '4'
  qtFlags['CPPPATH'] = [os.path.join(qtDir, 'include')]
  qtFlags['LIBPATH'] = [os.path.join(qtDir, 'lib')]
  qtFlags['LIBS'] = ['QtCore'+suffix, 'QtGui'+suffix, 'QtOpenGL'+suffix]
  qtMOC = os.path.join(qtDir, 'bin', 'moc.exe')
if buildOS == 'Darwin':
  qtFlags['CPPPATH'] = ['/usr/local/include']
  qtFlags['FRAMEWORKPATH'] = ['/usr/local/lib']
  qtFlags['FRAMEWORKS'] = ['QtCore', 'QtGui', 'QtOpenGL']
  qtMOC = '/usr/local/bin/moc'
if buildOS == 'Linux':
  qtFlags['CPPDEFINES'] = ['_DEBUG']
  qtFlags['CPPPATH'] = ['/usr/include']
  qtFlags['LIBPATH'] = ['/usr/lib']
  qtFlags['LIBS'] = ['QtGui', 'QtCore', 'QtOpenGL']
  qtMOC = '/usr/bin/moc-qt4'

if buildOS == 'Windows':
  env.Append(CCFLAGS = ['/Od', '/Zi']) # 'Z7'
  env['CCPDBFLAGS']  = ['${(PDB and "/Fd%s.pdb /Zi" % File(PDB)) or ""}']

fabricDir = os.environ['FABRIC_DIR']
fabricFlags = {
  'CPPDEFINES': ['FEC_SHARED'],
  'CPPPATH': [
    os.path.join(fabricDir, 'include'),
    os.path.join(fabricDir, 'include', 'FabricServices'),
    os.path.join(fabricDir, 'include', 'FabricUI'),
  ],
  'LIBPATH': [os.path.join(fabricDir, 'lib')],
  'LIBS': ['FabricCore-2.0', 'FabricServices']
}
uiLib = SConscript('SConscript',

  exports= {
    'parentEnv': env,
    'stageDir': env.Dir('#').Dir('stage'),
    'buildOS': buildOS,
    'buildArch': buildArch,
    'buildType': buildType,
    'qtMOC': qtMOC,
    'fabricFlags': fabricFlags,
    'qtFlags': qtFlags,
    'qtDir': qtDir,
    'uiLibPrefix': 'ui',
  },
  variant_dir = env.Dir('#').Dir('build').Dir('FabricUI'))

if buildOS == 'Windows':
  pdbFile = env.Dir('#').File('vc'+env['MSVC_VERSION'].replace('.', '')+'.pdb')
  env.Depends(pdbFile, uiLib)
  uiLib += env.InstallAs(env.Dir('#').Dir('stage').Dir('lib').File('FabricUI.pdb'), pdbFile)

env.Default(uiLib)
