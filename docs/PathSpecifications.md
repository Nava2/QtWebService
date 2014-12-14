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

Description     | Qt Web Syntax     | Regex Equivalent  | Matches
----------------|-------------------|-------------------|----------------------
Root Directory  | `/`               | `/`               | `/`
Multiple levels | `/foo/bar`        | `/foo/bar`        | `/foo/bar`


* Root Directory

    *QtWS:* `/` 

    *Regex:* `/`

    *Matches:* `/`
    
* Levels 
    
    *QtWS:* `/foo/bar`
    
    *Regex:* `/foo/bar`

    
    This only matches the two mentioned levels, i.e. `/foo/bar`. 

* Match the several names foo, bar or baz

    *QtWS:* `/foo|bar|baz`
    *Regex:* `/(?:foo|bar|baz)`

## Per-level syntax

All per-level syntax components can be composed to create a multiple level 
path. Each of the following examples are for a single level, multiple level examples are shown later. 

Description     | Qt Web Syntax     | Regex Equivalent  | Matches
----------------|-------------------|-------------------|----------------------
Multiple exact names | `/foo|bar|baz` | `/(?:foo|bar|baz)` | `/foo`, `/bar`, `/baz`
Match any character, one or more wide   | `/+` | `/(\U+)` | `/A`, `/BB`, `/CCC`, etc.
Match any character, zero or more wide  | `/*` | `/(\U*)` | `/`, `/a`, `/bb`, ...
Constant prefix | `/foo+`           | `/(foo\U+)`       | `/foo1`, `/foo2`, `/fooA`, ...
Constant optional suffix, optional start | `/*bar` | `/(\U*bar)` | `/foobar`, `/bar`, `/a-bar`, ...

**Note:** It is poor practice to use `*` as the only character in a match statement because it will match the previous path. It will lead to some frustrating bugs in your code. It is however useful for prefixes and suffixes. 

### Names and Grouping

Qt Web Syntax supports usage of names and grouping. Names are specified with the following syntax: `:name[$specification]`. The name may be any alphanumeric character, `-`, or `_`. These names are case sensitive and must not be repeated within a path specification. The `[$specification]` component is optional, it will default to `+` if unspecified -- implying one or more valid characters. The specification rules allow for all specifications used in a non-named level. 

Additionally, if a name is not specified and a Wildcard is used, the entire level will be captured in a "splat" which is an unamed group. Accute readers will note that in the [Per-level Syntax] section, the examples had Regex groups implied, these behaviours show the result of the wildcard usage. 

Description     | Qt Web Syntax     | Regex Equivalent  | Matches
----------------|-------------------|-------------------|----------------------
Name with default spec. | `/:name` | `/(?<name>\U+)` | `/foo` with `{"name" : "foo"}`, `/bar` with `{"name" : "bar"}`
Name with simple spec. | `/:name$foo` | `/(?<name>foo)` | `/foo` with `{"name" : "foo"}`
Name with OR + Wildcard | `/:name$*foo|bar+` | `/(?<name>\U*foo|bar\U+)` | `/foo` with `{"name" : "foo"}`, `/barA` with `{"name" : "barA"}`
