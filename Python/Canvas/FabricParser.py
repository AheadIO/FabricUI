
import argparse, sys, os
import os.path

def CheckExtension(choices):

    """CheckExtension

    Argparse action to check a file extension at loading.

    Arguments:
        choices (Dictionary): List of allowed extensions {'ext1, ext2, ...'}.

    """

    class Act(argparse.Action):
        def __call__(self, parser, namespace, fname, option_string=None):
            ext = os.path.splitext(fname)[1][1:]
            if ext not in choices:
                option_string = '({})'.format(option_string) if option_string else ''
                parser.error("File doesn't end with one of {}{}".format(choices,option_string))
            else:
                setattr(namespace,self.dest,fname)

    return Act


class FabricParser(argparse.ArgumentParser):

    """FabricParser

    Specialize the argparse.ArgumentParser class.
    If an error occurs when parsing the arguments, 
    the available options will be diplayed before exiting.

    """

    def error(self, message):
        sys.stderr.write('\n\nerror %s\n' %  message)
        self.print_help()
        sys.exit(2)
