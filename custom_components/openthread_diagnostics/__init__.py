import esphome.codegen as cg

CODEOWNERS = ["@local"]
DEPENDENCIES = ["openthread"]

openthread_diagnostics_ns = cg.esphome_ns.namespace("openthread_diagnostics")
OpenThreadDiagnostics = openthread_diagnostics_ns.class_("OpenThreadDiagnostics", cg.PollingComponent)
