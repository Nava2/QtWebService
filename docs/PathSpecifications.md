# Path Specifications

QtWebService uses a unix/regex-like language for specifying paths and routes. The language is significantly simpler 
than Regex and this was the intent. 

For example, the consider the following (overly complicated, partially contrived) example:
*QtWS:* `/:action$create|delete/:type$withdrawl|deposit|status/:userId$user+/`
*Regex:* 

## Basics

All examples are presented in the QtWebService route language followed by the regex version. Valid characters in a URL 
are represented in the Regex representations by `\U` which is not a valid escape character (substitute `\U` for 
`[\w\d\-_]`).

* Root Directory

    *QtWS:* `/` 
    *Regex:* `/`
    
* Levels 
    
    *QtWS:* `/foo/bar`
    *Regex:* `/foo/bar`
    
    This only matches the two mentioned levels, i.e. `/foo/bar`. 

* Match the several names foo, bar or baz

    *QtWS:* `/foo|bar|baz`
    *Regex:* `/(?:foo|bar|baz)`

## Wildcards

Wildcards only match the following: alphanumerics, `-` and `_`. No other characters match, this is represented in Regex 
by: `[\w\d\-_]` and within the guide, we will use `\U` to represent the previous regex. 
    
* Constant prefix: 

    *QtWS:* `/foo*`
    *Regex:* `/foo\U*`

* Constant suffix:

    *QtWS:* `/*oof`
    *Regex:* `/\U*oof`
    
    This will match `/oof` and `/baroof` just the same.

* Constant prefix and suffix:

    *QtWS:* `/foo*bar`
    *Regex:* `/foo\U*bar`
    
* Wildcards and OR
    
    *QtWS:* `/foo*|a+bar`
    *Regex:* `/(?:foo\U*|a\U+bar)/`

## Variables

All levels in a path may store a single variable. 

* Named variable for multiple options
    
    *QtWS:* `/:name$foo|bar`
    *Regex:* `/(?<name>foo|bar)/`
    
    In the Routed Response, the variable "name" can be accessed from the dictionary and will contain one of the two values specified. 
    
* Named variable with wildcards

    *QtWS:* `/:name$foo*|bar+`
    *Regex:* `/(?<name>foo\U*|bar\U+)/`
    