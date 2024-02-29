import argparse
import os
import shutil
from copy_files import copy_file, copy_files

def define_operations(out_dir, derived_sources_dir, project_source_dir, javascriptcore_derived_sources_dir, webinspectorui_dir):
    """Define copy operations for SDK components, including both directories and individual files."""
    return [
        {"type": "file", "src": f"{project_source_dir}/Source/WebCore/config.h", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/WebCore/"},
        
        {"type": "dir", "src": f"{derived_sources_dir}", "dst": f"{out_dir}", "include_patterns": ['*.h', '*.js']},
        
        {"type": "dir", "src": f"{project_source_dir}/Source/WebCore/platform/ultralight", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/WebCore/platform/ultralight", "include_patterns": ['*.h']},
        {"type": "dir", "src": f"{project_source_dir}/Source/WebCore/platform/graphics/ultralight", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/WebCore/platform/graphics/ultralight", "include_patterns": ['*.h']},
        {"type": "dir", "src": f"{project_source_dir}/Source/WebCore/platform/graphics/harfbuzz", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/WebCore/platform/graphics/harfbuzz", "include_patterns": ['*.h']},
        {"type": "dir", "src": f"{project_source_dir}/Source/WebCore/platform/graphics/texmap", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/WebCore/platform/graphics/texmap", "include_patterns": ['*.h']},
        
        {"type": "file", "src": f"{project_source_dir}/Source/WebCore/platform/graphics/ShadowBlur.h", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/WebCore/platform/graphics/"},
        {"type": "file", "src": f"{project_source_dir}/Source/WebCore/platform/MIMETypeRegistry.h", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/WebCore/platform/"},
        {"type": "file", "src": f"{project_source_dir}/Source/WebCore/platform/FileChooser.h", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/WebCore/platform/"},
        {"type": "file", "src": f"{project_source_dir}/Source/WebCore/platform/SharedTimer.h", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/WebCore/platform/"},
        {"type": "file", "src": f"{project_source_dir}/Source/WebCore/platform/MainThreadSharedTimer.h", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/WebCore/platform/"},
        
        {"type": "dir", "src": f"{project_source_dir}/Source/SQLite", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/SQLite", "include_patterns": ['*.h']},
        {"type": "dir", "src": f"{project_source_dir}/Source/WebCore/storage", "dst": f"{out_dir}/DerivedSources/ForwardingHeaders/WebCore/storage", "include_patterns": ['*.h']},
        
        # JavaScriptCore headers
        {"type": "file", "src": f"{project_source_dir}/Source/JavaScriptCore/API/JavaScript.h", "dst": f"{out_dir}/include/JavaScriptCore/"},
        {"type": "file", "src": f"{project_source_dir}/Source/JavaScriptCore/API/JSBase.h", "dst": f"{out_dir}/include/JavaScriptCore/"},
        {"type": "file", "src": f"{project_source_dir}/Source/JavaScriptCore/API/JSContextRef.h", "dst": f"{out_dir}/include/JavaScriptCore/"},
        {"type": "file", "src": f"{project_source_dir}/Source/JavaScriptCore/API/JSStringRef.h", "dst": f"{out_dir}/include/JavaScriptCore/"},
        {"type": "file", "src": f"{project_source_dir}/Source/JavaScriptCore/API/JSObjectRef.h", "dst": f"{out_dir}/include/JavaScriptCore/"},
        {"type": "file", "src": f"{project_source_dir}/Source/JavaScriptCore/API/JSTypedArray.h", "dst": f"{out_dir}/include/JavaScriptCore/"},
        {"type": "file", "src": f"{project_source_dir}/Source/JavaScriptCore/API/JSValueRef.h", "dst": f"{out_dir}/include/JavaScriptCore/"},
        {"type": "file", "src": f"{project_source_dir}/Source/JavaScriptCore/API/WebKitAvailability.h", "dst": f"{out_dir}/include/JavaScriptCore/"},
        {"type": "file", "src": f"{project_source_dir}/Source/JavaScriptCore/API/JSObjectRefPrivate.h", "dst": f"{out_dir}/include/JavaScriptCore/"},
        {"type": "file", "src": f"{project_source_dir}/Source/JavaScriptCore/API/JSRetainPtr.h", "dst": f"{out_dir}/include/JavaScriptCore/"},
        
        # WebInspectorUI directory and files
        {"type": "dir", "src": f"{webinspectorui_dir}/UserInterface/", "dst": f"{out_dir}/inspector"},
        {"type": "file", "src": f"{webinspectorui_dir}/Localizations/en.lproj/localizedStrings.js", "dst": f"{out_dir}/inspector"},
        {"type": "file", "src": f"{javascriptcore_derived_sources_dir}/inspector/InspectorBackendCommands.js", "dst": f"{out_dir}/inspector/Protocol"},
        {"type": "file", "src": f"{project_source_dir}/cacert.pem", "dst": f"{out_dir}/bin/resources/cacert.pem"},
    ]

def perform_copy_operations(operations):
    """Execute defined copy operations, handling both directories and files."""
    for op in operations:
        if op['type'] == 'dir':
            copy_files(op['src'], op['dst'], op.get('include_patterns', []), op.get('exclude_patterns', []))
        elif op['type'] == 'file':
            copy_file(op['src'], op['dst'])

def main(destination_dir, derived_sources_dir, project_source_dir, javascriptcore_derived_sources_dir, webinspectorui_dir):
    """Main function to orchestrate copying SDK files."""
    operations = define_operations(destination_dir, derived_sources_dir, project_source_dir, javascriptcore_derived_sources_dir, webinspectorui_dir)
    perform_copy_operations(operations)
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Copy WebCore SDK Files.')
    parser.add_argument('--destination-dir', required=True, help='Destination directory for the SDK components')
    parser.add_argument('--derived-sources-dir', required=True, help='Directory for derived sources')
    parser.add_argument('--project-source-dir', required=True, help='Directory for the project source')
    parser.add_argument('--javascriptcore-derived-sources-dir', required=True, help='Directory for JavaScriptCore derived sources')

    args = parser.parse_args()
    
    webinspectorui_dir = os.path.join(args.project_source_dir, 'Source/WebInspectorUI')

    main(args.destination_dir, args.derived_sources_dir, args.project_source_dir, args.javascriptcore_derived_sources_dir, webinspectorui_dir)
