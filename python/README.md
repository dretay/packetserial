conda create -n packetserial python=3.11
conda activate packetserial
#NOTE: must be swig > 4.2
conda install pip jupyter conda-forge::swig
pip install greatfet
cd swig
apt install build-essential
cd ..
jupyter notebook