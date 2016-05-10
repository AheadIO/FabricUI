"""
The FabricParser handles parsing command line options for the Canvas standalone.
"""

import argparse, sys, os
import os.path

def CheckExtension(choices):

    """Argparse action to check a file extension at loading.

    Arguments:
        choices (Dictionary): List of allowed extensions {'ext1, ext2, ...'}.

    """

    class Act(argparse.Action):
        def __call__(self, parser, namespace, fname, option_string=None):
            if fname is None:
                setattr(namespace,self.dest,None)

            else:
                ext = os.path.splitext(fname)[1][1:]
                if ext not in choices:
                    option_string = '({})'.format(option_string) if option_string else ''
                    parser.error(str(ext) + " isn't supported, please use " + str(choices) + " files")
                else:
                    setattr(namespace,self.dest,fname)

    return Act


class FabricParser(argparse.ArgumentParser):

    """Specialize the argparse.ArgumentParser class.
    If an error occurs when parsing the arguments, 
    the available options will be diplayed before exiting.
    """

    def error(self, message):
        sys.stderr.write('\n\nerror %s\n' %  message)
        self.print_help()
        sys.exit(2)
