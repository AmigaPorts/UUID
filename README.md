# UUID library and utilities

This is a RFC 4122 compatible uuid library. It supports both time- and
random-based UUID generators as well as few tiny utilities. The library is
missing the ability to obtain MAC address (used as the node element of UUID).
Instead, the node is generated from random noise.

This work has been bassed on the RFC 4122 draft 
(http://www.apps.ietf.org/rfc/rfc4122.html) and the uuid library from 
the e2fsprogs package.
