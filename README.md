# openbsd volume query-er

this is a simple test driver i built to help me re-write the volume querying
logic in [oxbar](https://github.com/ryanflannery/oxbar). all it does is query
the left/right and muted status the outputs.master mixer device.

if you're parsing mixer devices/control objects in openbsd, this may be of help.
