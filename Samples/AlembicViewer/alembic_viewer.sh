echo "========================"
echo "Launching Alembic Viewer"
echo "========================"
echo ""

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
FABRIC_DIR=$DIR/../../..

source $FABRIC_DIR/environment.sh

PYTHONPATH=$DIR/../:$PYTHONPATH
export PYTHONPATH

python alembic_viewer.py