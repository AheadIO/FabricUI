#
# Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
#

import os, platform, subprocess
from distutils import spawn

AddOption('--buildType',
                  dest='buildType',
                  type='string',
                  nargs=1,
                  action='store',
                  help='Type of build to perform (Release or Debug)')

if not os.environ.has_key('FABRIC_DIR'):
  raise Exception("No FABRIC_DIR environment variable specified.")

buildOS = 'Darwin'
if platform.system().lower().startswith('win'):
  buildOS = 'Windows'
elif platform.system().lower().startswith('lin'):
  buildOS = 'Linux'

buildArch = 'x86_64'
buildType = 'Release'

if str(GetOption('buildType')).lower() == 'debug':
  buildType = 'Debug'

env = Environment(MSVC_VERSION = "12.0")
env.Append(CPPPATH = [env.Dir('#').srcnode().abspath])

qtDir = os.environ['QT_DIR']
qtMOC = os.path.join(qtDir, 'bin', 'moc')
qtFlags = {
  'CPPPATH': [os.path.join(qtDir, 'include')],
  'LIBPATH': [os.path.join(qtDir, 'lib')],
  }

if buildOS == 'Windows':
  qtMoc += '.exe'
  if buildType == 'Debug':
    suffix = 'd4'
  else:
    suffix = '4'
  qtFlags['LIBS'] = ['QtCore'+suffix, 'QtGui'+suffix, 'QtOpenGL'+suffix]
if buildOS == 'Darwin':
  qtFlags['FRAMEWORKPATH'] = [os.path.join(qtDir, 'lib')]
  qtFlags['FRAMEWORKS'] = ['QtCore', 'QtGui', 'QtOpenGL']
if buildOS == 'Linux':
  qtFlags['CPPDEFINES'] = ['__STDC_CONSTANT_MACROS']
  qtFlags['LIBS'] = ['QtGui', 'QtCore', 'QtOpenGL']
  env.Append(CXXFLAGS = ['-fPIC'])
  if buildType == 'Debug':
    env.Append(CXXFLAGS = ['-g'])
    env.Append(LDFLAGS = ['-g'])

def GetPythonSysConfigVar(python, varname):
  p = subprocess.Popen([python, "-c", """
try:
  import sysconfig
except:
  from distutils import sysconfig
print sysconfig.get_config_var('%s')
""" % varname], stdout=subprocess.PIPE)
  out, err = p.communicate()
  return out.strip()

pythonVersion = '2.7'
pysideVersion = '1.2.4'

if buildOS == 'Windows':
  import _winreg
  for root in [_winreg.HKEY_LOCAL_MACHINE, _winreg.HKEY_CURRENT_USER]:
    try:
      pythonInstallPath = _winreg.QueryValue(
        _winreg.OpenKey(
          _winreg.OpenKey(
            _winreg.OpenKey(
              _winreg.OpenKey(root, "Software"),
              "Python"
              ),
            "PythonCore"
            ),
          pythonVersion
          ),
        "InstallPath"
        )
      break
    except:
      pythonInstallPath = ''
  if len(pythonInstallPath) == 0:
    raise Exception('unable to find Python install path using winreg')
  python = os.path.join(pythonInstallPath, "python.exe")
else:
  python = "python%s" % pythonVersion

if not spawn.find_executable(python):
  raise Exception("cannot find executable %s" % python)

includeDir = GetPythonSysConfigVar(python, 'INCLUDEPY')

if buildOS == 'Windows':
  lib = 'python' + GetPythonSysConfigVar(python, 'VERSION')
  libDir = os.path.join(pythonInstallPath, 'libs')
else:
  lib = 'python%s' % pythonVersion
  libDir = GetPythonSysConfigVar(python, 'LIBDIR')

moduleSuffix = GetPythonSysConfigVar(python, 'SO')

pythonConfigs = {}
pythonConfigs[pythonVersion] = {
  'python': python,
  'includeDir': includeDir,
  'libDir': libDir,
  'lib': lib,
  'moduleSuffix': moduleSuffix,
  'pythonFlags': {
    'CPPPATH': [includeDir],
    'LIBPATH': [libDir],
    'LIBS': [lib]
    },
  }
  
# shiboken+pyside
pysidePkgDir = env.Dir(os.environ['SHIBOKEN_PYSIDE_DIR'])
pysideFlags = {
  "CPPPATH": [
    pysidePkgDir.Dir('include'),
    pysidePkgDir.Dir('include/PySide'),
    pysidePkgDir.Dir('include/PySide/QtCore'),
    pysidePkgDir.Dir('include/PySide/QtGui'),
    pysidePkgDir.Dir('include/PySide/QtOpenGL'),
    ],
  "LIBPATH": [pysidePkgDir.Dir('lib')],
  "LIBS": ["pyside-python"+pythonVersion],
}
shibokenFlags = {
  "CPPPATH": [pysidePkgDir.Dir('include').Dir('shiboken')],
  "LIBPATH": [pysidePkgDir.Dir('lib')],
  "LIBS": ["shiboken-python"+pythonVersion],
}
pysideIncludeDir = pysidePkgDir.Dir('include').Dir('PySide')
pysideTypesystemsDir = pysidePkgDir.Dir('share').Dir('PySide').Dir('typesystems')

if buildOS == 'Windows':
  shibokenBin = pysidePkgDir.Dir('bin').File('shiboken.exe')
else:
  shibokenBin = pysidePkgDir.Dir('bin').File('shiboken')

pythonConfigs[pythonVersion]['shibokenBin'] = shibokenBin
pythonConfigs[pythonVersion]['shibokenFlags'] = shibokenFlags
pythonConfigs[pythonVersion]['pysideIncludeDir'] = pysideIncludeDir
pythonConfigs[pythonVersion]['pysideTypesystemsDir'] = pysideTypesystemsDir
pythonConfigs[pythonVersion]['pysideFlags'] = pysideFlags
pythonConfigs[pythonVersion]['pysideDir'] = pysidePkgDir
pythonConfigs[pythonVersion]['pysideVersion'] = pysideVersion
pythonConfigs[pythonVersion]['havePySide'] = True

if buildOS == 'Windows':
  env.Append(CCFLAGS = ['/Od', '/Zi']) # 'Z7'
  env['CCPDBFLAGS']  = ['${(PDB and "/Fd%s.pdb /Zi" % File(PDB)) or ""}']
  env.Append(CCFLAGS = ['/FS'])

fabricDir = os.environ['FABRIC_DIR']
fabricFlags = {
  'CPPDEFINES': ['FEC_SHARED', "ENABLE_STYLERELOAD"],
  'CPPPATH': [
    '..',
    os.path.join(fabricDir, 'include'),
    os.path.join(fabricDir, 'include', 'FabricServices'),
    os.path.join(fabricDir, 'include', 'FabricServices', 'ASTWrapper'),
    os.path.join(fabricDir, 'Python', pythonVersion, 'FabricEngine'),
  ],
  'LIBPATH': [
    os.path.join(fabricDir, 'lib'),
    os.path.join(fabricDir, 'Python', pythonVersion, 'FabricEngine'),
  ],
  'LIBS': ['FabricCore', 'FabricServices'],
}

stageDir = env.Dir('stage')
env['STAGE_DIR'] = stageDir

uiLib = SConscript('SConscript',
  exports= {
    'parentEnv': env,
    'stageDir': stageDir,
    'buildOS': buildOS,
    'buildArch': buildArch,
    'buildType': buildType,
    'qtMOC': qtMOC,
    'fabricFlags': fabricFlags,
    'qtFlags': qtFlags,
    'qtDir': qtDir,
    'uiLibPrefix': 'ui',
    'pythonConfigs': pythonConfigs,
    'capiSharedLibFlags': {},
    'servicesFlags_mt': {},
    'corePythonModuleFiles': [],
  },
  variant_dir = env.Dir('#').Dir('build').Dir('FabricUI'),
  duplicate=0)

if buildOS == 'Windows':
  pdbFile = env.Dir('#').File('vc'+env['MSVC_VERSION'].replace('.', '')+'.pdb')
  env.Depends(pdbFile, uiLib)
  uiLib += env.InstallAs(env.Dir(fabricDir).Dir('lib').File('FabricUI.pdb'), pdbFile)

env.Default(uiLib)
