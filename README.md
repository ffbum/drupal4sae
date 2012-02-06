-- 简介 --

可在新浪 App Engine（SAE）上安装并正常使用Drupal 7.x.

Demo测试站点及最新更新：
  http://drupaldev.sinaapp.com/
相关模块项目主页：
  http://drupal.org/project/sae
提交Bug或者提供建议，请访问：
  http://drupal.org/project/issues/sae


-- 安装要求 --

* 本Drupal必须安装在SAE上


-- 安装指南 --

* 下载最新版本的Drupal for SAE

* 在SAE登录并创建你的应用

*  把Drupal for SAE通过SVN部署到SAE，参见SAE的入门指引： http://sae.sina.com.cn/?m=devcenter&catId=212

* 进入SAE应用管理页面，打开 服务信息 >> Storage，新建一个domain，命名为sites，并且不设置私有和缓存过期

* 在浏览器中访问： http://你应用域名/install.php 你的应用域名通常是: AppID.sinaapp.com 这样的格式。

* 在第一个页面，选择“Sina App Engine”

* 然后保存并继续，可以选择要安装的Drupal站点语言，目前支持原生英文和中文。 

* 保存后就完成了Drupal的安装。

图文版安装指南参见：http://drupaldev.sinaapp.com/node/1

-- 配置指南 --

* 你可以在 管理 >> 用户 >> 访问控制 >> Sina App Engine模块里设置用户访问SAE配置页面的权限。

  只有拥有“管理SAE设置”权限的用户能够访问SAE配置页面。

* 你可以在 管理 >> 配置 >> SAE邮件 页面配置你的SMTP服务器设置。
  只有配置好SMTP服务器之后才能通过Drupal发送邮件。
    
  
-- 已知问题 --
  这个模块并不完美，因为SAE有太多的限制，而我又不可能测试Drupal的所有功能。我只能尽量避免修改
  系统文件，这样需要的时候仍然可以升级Drupal。
  
  即使对drupal_http_request()函数打了补丁，SAE下的fsockopen函数也不支持SSL协议，所以不能
  来获取HTTPS页面。
  
  SAE通过Storage系统来储存用户上传的文件，而代码目录不可写，所以某些使用了代码目录的模块可能会
  出错，如果错误不影响功能，忽略就好。如果有影响，请把Bug提交给我。
  
  SAE上的临时目录也是临时生成的，所以不能在配置页面修改。私有文件目录暂时没有测试是否可用，所以先
  禁用了。
  
  使用图像或者文件操作的时候，因为SAE上文件操作速度较慢，如果慢于代码执行速度就会出错，比如上传
  文件时filesize()函数就可能出错不能获取文件大小信息。不过这并不影响正常上传，一般来说没什么关系。
  
  Testing模块需要open_dir的支持，所以在SAE上不能使用，也就没办法用这个模块来测试其它模块是否
  正常了，只能在实践中检验。如果你发现有什么错误，请提交Bug给我。

-- 作者 --

作者:
* ffbum - http://blog.ykfan.cn/blackhole
  
  yuanyeff@gmail.com
