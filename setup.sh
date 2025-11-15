echo installing 
echo installing cmake
sudo apt install cmake -y
echo 

#stb
echo installing STB
git clone https://github.com/nothings/stb.git
echo STB install done

# GLM
echo installing GLM
git clone https://github.com/g-truc/glm.git
echo GLM Install done

#XrandR
echo installing XrandR for Resolution changes
sudo apt install libxrandr-dev -y
echo  Xrandr installed

echo installing libopenAL and sound libs
sudo apt install libopenal-dev -y
sudo apt install pulseaudio -y
sudo apt install libalut-dev -y
sudo apt install libogg-dev -y
sudo apt install libvorbis-dev -y
sudo apt install vorbis-tools -y
echo we have installed sound libs, not all are needed

echo get latest mesa libs 
sudo apt install libgles2-mesa-dev -y
echo libmesa is Done

echo FreeType installed
echo installing Assimp
cd..
sudo apt install libassimp-dev
echo Assimp installed 

echo enhancing GPU debug output not all will install
sudo apt install libglx-mesa0-dbgsym libglapi-mesa-dbgsym libgl1-mesa-dri-dbgsym
echo Done!
echo
echo Scroll up to make sure there were no errors, warnings are ok
echo Take note of the errors and check the web/support sites for fixes
read -s -p "Press Enter when you're ready!"
echo ...