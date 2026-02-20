# Supermon-7.4 AND Allmon3 announcement-creation-and-management-of-cron
Supermon 7.4+ and Allmon3 announcement creation and management of cron entries<br>
# WARNING...WARNING...WARNING
You will have the ability to create announcements that will play locally on YOUR node only or GLOBALLY on ALL CONNECTED NODES. It is YOUR responsibility to use this wisely and not to cause unwanted interference with other nodes. 

This works on rPi as well as dell wyse thin clients. if it has x86/amd or aarch or ARM64 architecture it should work. please let me know if it doesn't.
<br>
This feature was inspired by KD5FMU, Freddie Mac, the Ham Radio Crusader. His Audio_Convert and Playaudio script files are the basis for this feature.You do not need to install his versions before installing this as this will handle the install for its own needs. 

<img width="1920" height="1032" alt="image" src="https://github.com/user-attachments/assets/eab34d79-fa91-4167-a685-07ddff01934f" />
<img width="1920" height="1032" alt="image" src="https://github.com/user-attachments/assets/808fc895-4a14-4b8f-9860-ba3f1347a71a" />
<img width="1920" height="1020" alt="image" src="https://github.com/user-attachments/assets/128a639e-50b1-4481-9561-d2fef071d701" />
<img width="1920" height="1020" alt="image" src="https://github.com/user-attachments/assets/128a639e-50b1-4481-9561-d2fef071d701" />

Supermon is NOT a requirement for this program to work. It WAS made for supermon first, but Allmon3 was added to it. 
So if you do not have Supermon 7.4+ installed, you will need to install apache2 to handle the scripts. Here is how you do that. You will NOT need to do this if Supermon7.4+ is installed as the apache2 handler will be installed with Supermon7.4+.  NOW TAKE NOTE: if you decide to install Supermon later, then you will need to run this installer again to update the control panel in Supermon for this to show up. 
Run these lines individually.

```
sudo apt update && sudo apt upgrade -y
sudo apt install apache2 libapache2-mod-php php php-common -y
sudo systemctl enable --now apache2

```




Here is how to install it:
first lets get to s specific directory after you after started an SSH session into your node or from the terminal CLI if you like. 

```
cd /etc/asterisk/local
```

if the directory does not exist, lets create it

```
sudo mkdir /etc/asterisk/local
```

then switch to the directory

```
cd /etc/asterisk/local
```

Then lets download the installer script

```
sudo wget https://raw.githubusercontent.com/N5AD/announcement-manager/refs/heads/main/announcement_manager.sh
```
Then lets run the script using
```
sudo bash announcement_manager.sh
```
Then follow the promts.

The cron table will only show the announcements that were created by this program. If you have announcements that you created by other means, they will not show up here.
You can also schedule announcements for non standard times such as running an announcement on the second tuesday of every month. Just follow the prompts when you schedule the announcement. 
You can also create an MP3 of your own voice and save that in the MP3 directory. This directory is user accessable via ssh and  you do not need any special permissions to place files there. 
