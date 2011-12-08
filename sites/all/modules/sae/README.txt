
-- SUMMARY --

Adds support for runs Drupal on SAE(Sina App Engine).

For a full description of the module, visit the project page:
  http://drupal.org/project/sae
To submit bug reports and feature suggestions, or to track changes:
  http://drupal.org/project/issues/sae


-- REQUIREMENTS --

* Install Drupal on Sina App Engine

* Some Core Hack for Drupal
  http://drupal.org/project/libraries



-- INSTALLATION --

* Install Drupal 7.x on SAE through SVN: http://sae.sina.com.cn/?m=devcenter
 
  You might need apply patch to modules/system/system.install first to install 
  Drupal on SAE normally.
 
* Download and install SAE module to your Drupal site on SAE, Enable it.
 
  Going to Administer >> Site configuration >> SAE Mail. Set a SMTP server and 
  account for send mail.
  
  Clear Cache on Adminster >> Development >> Performences.

-- CONFIGURATION --

* Configure user permissions at Administer >> User management >> Access
  control >> Sina App Engine module.

  Only users with the "administer saemailer settings" permission are allowed to
  access the module configuration page.

* Configure your SMTP server settings at Administer >> Site
  configuration >> SAE Mail.

  Set a SMTP server and account for send mail.
  
* Create your public file path manually.

  You must create you public file directory manually because SAE can't create
  directories from. In default, this path might be sites/all/default/files.
  Normally you need mannully create files directory and commit to SAE SVN.
  
  And you must set a storage domain in SAE configration page. This domain name
  must be the first patch of your public file path. In default, it's sites.
  Create a domain named sites and set it to public and no exprie.

* To use clean url on Drupal SAE, there's a config file had to be set.
  
  Copy config.yaml in sae module direcotry to your Drupal root. Edit this file 
  and change the first line form "name: drupaldev" to "name: appID".
  yourappname is the sub domain name of your SAE app. It can be set from SAE
  configration page. 
  
  Then add config.yaml to SVN and commit to SAE. Now you can enable clean url
  from Drupal administration page. But some module like image style doesn't
  support clean url yet, you might no use this feature if you wan to display
  image on your site.
  
-- PATCHS --

  You may run Drupal on SAE with almost modules and themes when enable SAE
  module. But there is still some error infomation display on the sreen. You may
  choose ignore it or patch some Drupal core files fix it.
  
  Open files with your text editor:
  
* includes/bootstrap.inc
  
  Search for "ini_set" and replace them to "@ini_set".
  
  This patch will bypass the error when Druapl call ini_set function to set the
  Session.
  
* includes/common.inc
  
  Search for "$socket = 'tcp://' . $uri['host'] . ':' . $port;" and replace 
  with "$socket = 'tcp://' . $uri['host'];".
  
  Search for "$socket = 'ssl://' . $uri['host'] . ':' . $port;" and replace 
  with "$socket = 'ssl://' . $uri['host'];".
  
  Search for "@stream_socket_client($socket, " and replace with
  "$fp = @fsockopen($socket, $port, ".
  
  This patch will use fsockopen instead of stream_socket_clien because SAE do
  not support stream functions. This patch is used when Update module and some 
  module like aggreator wants to fetch a url.
  
* modules/system/system.install

  Search for "$is_writable = is_writable($directory);" and replace with
  "$is_writable = TRUE;".
  
  Search for "$is_directory = is_dir($directory);" and replace with
  "$is_directory = TRUE;".
  
  This patch will bypass file check on Drupal file path. Then you will not see
  the file system error message in Drupal status page.
  
-- KOWN ISSUE --
  This is not a PERFECT adapter to Drupal on Sina App Engine. Because there are
  too many restrictions on SAE and need for Drupal. I don't want hack too much
  of Drupal core files.
  
  Drupal use sock stream functions in drupal_http_request to fetch page. I 
  change this back to fsockopen function and it works right. But becouse of 
  fsockopen on SAE can't support SSL Protocol, you can't use drupal_http_request
  to fetch a HTTPS page.
  
  SAE store files on a Storage Engine but code directory. So some php file
  functions can't support local path, like is_writable, chmod, and so. If there 
  is an error message said some directory can't be found, if no functions been
  affected, just ignore it.
  
  For the same reason, private file path and temporary path could not been set.
  
  When use file field or image field to update file and image. Sometime file
  oprations is slowly than php code, so some file operations will failed, like
  filesize() function. You could try upload again or ignore it.
  
  Testing module can't enable because SAE unsupport open_dir. So I can't test
  all modules by simpletest. If you find some modules is incompatible with SAE,
  just submit a bug to me Please.

-- CREDITS --

Authors:
* Ye Yuan - http://blog.ykfan.cn/blackhole

