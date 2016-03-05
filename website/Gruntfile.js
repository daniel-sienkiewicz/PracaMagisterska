module.exports = function(grunt) {
 
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),

    less: {
        development: {
            options: {
                paths: ["public/**"],
                yuicompress: true
            },
            files: {
                "public/css/styl.css": "public/css/styl.less"
            }
        }
    },
    watch: {
        files: ["public/css/*", "*.js", "public/**/*.js", 'routes/**/*.js'],
        tasks: ["less"]
    }
  });
  // Each plugin must be loaded following this pattern
  grunt.loadNpmTasks('grunt-contrib-less');
  grunt.loadNpmTasks('grunt-contrib-watch');
 
  grunt.registerTask('default', ['watch', 'less']);
};