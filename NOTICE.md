# Third-Party Notices

NotebookFlow is licensed under the GNU Affero General Public License v3.0
(see `LICENSE`). It also includes the following third-party components,
each under its own permissive license, reproduced below as required by
their terms.

---

## cpp-httplib

Location: `libUtil/httplib/httplib.h`

```
Copyright (c) 2026 Yuji Hirose. All rights reserved.
MIT License
```

Full license text: https://github.com/yhirose/cpp-httplib/blob/master/LICENSE

---

## json-c (adapted)

Location: `libUtil/json/` (`JSON_OBJECT.*`, `JSON_TOKENER.*`,
`JSON_LINKHASH.*`, `JSON_ARRAYLIST.*`, `JSON_PRINTBUF.*`, `JSON_UTIL.*`,
`JSON_DEBUG.*`, `JSONK.*`, `RESTMSG.*`)

These files were adapted from the [json-c](https://github.com/json-c/json-c)
project (ported to C++ under the `nsUtil` namespace, with additional
extensions such as `RestMsg`/`RestParam`). The original json-c project is
distributed under the MIT License, reproduced in full below (from
json-c's `COPYING` file, which itself combines two historical copyright
notices):

```
Copyright (c) 2009-2012 Eric Haszlakiewicz

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

----------------------------------------------------------------

Copyright (c) 2004, 2005 Metaparadigm Pte Ltd

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## Python dependencies (agent-flow, ssh-flow, mcp-flow, file-ra)

FastAPI, uvicorn, pydantic, paramiko, and other Python packages used by
the RA-layer services are third-party packages distributed under their
own permissive licenses (MIT/BSD/Apache-2.0), unmodified and used as
installed dependencies (not vendored source). See each package's own
distribution for its license.

## Node / frontend dependencies

React, Vite, and other `frontend/` npm dependencies are third-party
packages under their own licenses (used as installed dependencies via
`node_modules/`, not vendored source, and excluded from this repository
via `.gitignore`).
