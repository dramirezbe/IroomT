// handleJSON.js
const fs = require('fs');
const path = require('path');

/**
 * @param {string} dirPath
 *   Absolute path to the folder with JSON files named "0", "1", "2", …
 */
module.exports = function createJSONReader(dirPath) {
  let currentIndex = 0;

  /**
   * On each invocation, do up to 10 instantaneous attempts:
   *  - Read/parse at currentIndex.
   *  - On success: callback(null, data) and currentIndex++ for next tick.
   *  - On failure: if currentIndex!==0, reset to 0 and retry immediately.
   *    Otherwise keep trying at 0.
   *  - If 10 straight failures, callback(error).
   */
  return function readJSONWithRetries(callback) {
    let lastError = null;

    for (let attempt = 1; attempt <= 10; attempt++) {
      const filePath = path.resolve(dirPath, String(currentIndex));

      try {
        const jsonString = fs.readFileSync(filePath, 'utf8');
        const parsed = JSON.parse(jsonString);

        // success: prepare for next tick
        currentIndex++;
        return callback(null, parsed);

      } catch (err) {
        lastError = err;
        // if we weren’t already at 0, reset and retry quickly at 0
        if (currentIndex !== 0) {
          currentIndex = 0;
        }
        // else stay at 0 and retry immediately
      }
    }

    // after 10 tries, give up
    callback(lastError);
  };
};
